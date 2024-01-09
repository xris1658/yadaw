import QtQml

QtObject {
    enum Config
    {
        Custom,
        Empty,
        Mono,
        Stereo,
        LRC,
        Quad,
        C50,
        C51,
        C61,
        C71
    }
    QtObject {
        id: impl
        property var channelCounts: [0, 0, 1, 2, 3, 4, 5, 6, 7, 8]
    }
    function channelCount(config: int) {
        return impl.channelCounts[config];
    }
}
