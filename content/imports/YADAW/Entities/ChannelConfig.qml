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
    function channelCount(config: int) {
        const channelCounts = [0, 0, 1, 2, 3, 4, 5, 6, 7, 8];
        return impl.channelCounts[config];
    }
}
