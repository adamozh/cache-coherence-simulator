class CacheLine {
    unsigned int state; // The states currently will corresponds to 0,1,2,3 
    // M -- 0, E -- 1, S -- 2, I -- 3 or M -- 0 E -- 1 Sc -- 2 Sm -- 3
    unsigned int tag;
    friend CacheSet;

    CacheLine(unsigned int tag, unsigned int state){};
};