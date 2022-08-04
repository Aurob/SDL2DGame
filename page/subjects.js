/**
 * subjects.js 
 * Version: 3
 * Builds a basic configurable webpage from a YAML config file
 */

var subjects = {}
var subject_icons = {};
var subject_scripts = {};
var default_subject;
var default_logo;
var media_path;
var pageid;

$(document).ready(function () {
    if (window.location.hash) {
        pageid = window.location.hash.slice(1);
    }
    else pageid = "#";
    init(pageid);
});

function init(pageid) {
    //populate the subjects array
    fetch(`page/subjects.yml`, {
        method: 'GET', // or 'PUT'
        headers: {
            'Content-Type': 'text/yaml',
        }
    })
    .then(res => res.text())
    .then(yaml => jsyaml.load(yaml))
    .then(data => load_subjects(data))
    .then(events => {
        console.log(subjects);
        if ('_global' in subjects) update_page('_global');

        update_page(pageid);
        loadLinkEvents();
    })
}

function loadLinkEvents() {
    $(".link").on('click', function (e) {
        let id = e.target.hash.slice(1);
        update_page(id);
    });
}

function update_page(id) {
    window.location.hash = id;
    //Having existing search parameters when changing pages
    // may cause the page to load incorrectly.
    //Remove the search parameters from the URL, which causes a page reload.
    if (window.location.search) window.location.search = "";

    load_page(id);
}
//Loads a specific subject page
function load_page(id) {
    //redirect blank paths to home
    if (!(id in subjects)) {
        id = default_subject;
    }

    if (subjects[id] && "nav_logo" in subjects[id]) {
        console.log(id);
        if (subjects[id].nav_logo.endsWith(".js")) {
            $("#logo").parent().append("<script src='" + subjects[id].nav_logo + "'><\/script>");
        }
        else {
            $("#logo")[0].src = media_path + subjects[id].nav_logo;
        }
    }
    else $("#logo")[0].src = media_path + default_logo;

    if (subjects[id]) {
        $("#subject_block").html(subjects[id].block);
        if (id in subject_scripts) {
            for (let k = 0; k < Object.keys(subject_scripts[id]).length; k++) {
                let key = Object.keys(subject_scripts[id])[k];

                let script = subject_scripts[id][key];

                if (script.endsWith(".js"))
                    $.getScript(script);
                else if (script.startsWith("<script>") && script.endsWith("<\/script>")) {
                    $("#subject_block").append(script);
                }
                else {
                    $("#subject_block").append("<script>" + script + "<\/script>");
                }
            }

        }

        // $("#subject_block").css("width", subjects[id].width)
    }
    $(".page").css("display", "none");
    $("#" + id).css("display", "flex");

}

async function load_subjects(data) {
    if ("header" in data) {
        if ("title" in data.header) {
            document.title = data.header.title;
        }
        if ("nav_logo" in data.header) {
            default_logo = data.header.nav_logo;
        }
        if ("media_path" in data.header) {
            media_path = data.header.media_path;
        }

        if ("style" in data.header) {
            if (data.header.style.endsWith(".css"))
                $("head").append("<link rel='stylesheet' href='" + data.header.style + "'>");
            else
                $("head").append("<style>" + data.header.style + "<\/style>");
        }

        if ("script" in data.header) {
            let script = data.header.script;
            if (script.endsWith(".js"))
                $("head").append("<script src='" + script + "'><\/script>");
            else if (script.startsWith("<script>") && script.endsWith("<\/script>")) {
                $("head").append(script);
            }
            else {
                $("head").append("<script>" + script + "<\/script>");
            }
        }

        $("body").css("width", data.header.width)
    }

    for (subject in data.data) {
        await load_subject(subject, data.data[subject]);
    }
}

async function load_subject(id, subject) {
    subject.id = id;

    for(let k = 0; k < Object.keys(subject).length; k++) {
        let key = Object.keys(subject)[k];
        if (key.indexOf("script") > -1) {
            if (!subject_scripts[subject.id]) subject_scripts[subject.id] = [];
            subject_scripts[subject.id].push(subject[key]);
        }
    }
    if ("default" in subject && subject.default) {
        default_subject = subject.id;
    }

    if (subject.id != '_global') {
        //Add subject to nav list
        if (subject.link) href = subject.link;
        else href = '#' + subject.id;
        $("#nav-list").append(`<div class="flex-item">
                        <a class="link" href="${href}">${subject.subject}</a>
                    </div>`);

        //Contstruct subject body and add to subjects array
        let subject_header =
            '<div id="header">' +
            '<h1>' + subject.title + '</h1>' +
            '<small>' + subject.sub_title + '</small>' +
            '<hr>'
        '</div>';

        var subject_content = [];

        for (i = 0; i < Object.keys(subject).length; i++) {
            let key = Object.keys(subject)[i];
            if (key.startsWith('content')) {
                await load_html_template(key, String(subject[key])).then(content => {

                    subject_content.push(content);
                });
            }
            if (key.startsWith("style")) {
                await load_stylesheet(subject[key]).then(styles => {
                    subject_content.push(styles);
                });
                // console.log(key)
                // let styles = subject[key];
                // let stylesheet = $(`<style>${styles}</style>`);
                // // $("head").append(stylesheet);
                // subject_content.push(stylesheet);
            }
        }

        subject_content = subject_content.join('');
        let subject_image = '';

        if (subject.image) {
            if (typeof (subject.image) == "string") {
                subject_image = '<img loading=\'lazy\' src="' + media_path + subject.image + '"</img>';
            }
            else {
                for (image in subject.image) {
                    subject_image += '<img loading=\'lazy\' src="' + media_path + subject.image[image] + '"</img>';
                }
            }
        }

        let subject_block =
            '<div id="' + subject.id + '" class="page flex-item">' +
            subject_header +
            ((subject.image != '') ? subject_image : '') +
            '</div>'
            +
            subject_content;

        subject.block = subject_block;
    }

    subjects[subject.id] = subject;
}

//Asynchronously loads an HTML file
// returns the content as-is if it's a simple string
async function load_html_template(key, content) {
    let div = $(`<div id='${key}'></div>`);
    if (content.endsWith(".html")) {
        if (key.endsWith("iframe")) {
            div.append(`<iframe src='${content}'></iframe`);
        }
        else {
            div.append(await fetch(content)
                .then(data => data.text())
                .then(html => {
                    return html
                }));
        }
    }
    else div.append(content);

    return div[0].outerHTML;
}

//Asynchronously loads a stylesheet
// returns the content as-is if it's a simple string
async function load_stylesheet(content) {
    let style = $(`<style></style>`);
    if (content.endsWith(".css")) {
        style.append(await fetch(content)
            .then(data => data.text())
            .then(stylesheet => {
                return stylesheet
            }));
    }
    else style.append(content);

    return style[0].outerHTML;
}
