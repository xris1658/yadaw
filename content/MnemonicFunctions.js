function mnemonicText(input/*: string*/) {
    let ret = input.indexOf('&') >= 0?
        input.split('&')[0] + input.split('&')[1].substring(0, 1) + input.split('&')[1].substring(1, input.split('&')[1].length):
        input;
    return ret;
}
function mnemonicTextWithUnderline(input/*: string*/) {
    let ret = input.indexOf('&') >= 0?
        input.split('&')[0] + "<u>" + input.split('&')[1].substring(0, 1) + "</u>" + input.split('&')[1].substring(1, input.split('&')[1].length):
        input;
    return ret;
}

function removeMnemonicFromText(input, regex/*: string*/, replaceWith/*: string*/) {
    return input.replace(new RegExp(regex), replaceWith);
}