function mnemonicText(input) {
    let ret = input.indexOf('&') >= 0?
        input.split('&')[0] + input.split('&')[1].substring(0, 1) + input.split('&')[1].substring(1, input.split('&')[1].length):
        input;
    return ret;
}
function mnemonicTextWithUnderline(input) {
    let ret = input.indexOf('&') >= 0?
        input.split('&')[0] + "<u>" + input.split('&')[1].substring(0, 1) + "</u>" + input.split('&')[1].substring(1, input.split('&')[1].length):
        input;
    return ret;
}