[[link(android)]]
class Test {
    int member(std::string str);
    int member2(std::string str);
    ///Woah

    // result: win 0x433, ios inline, imac default, m1 default
    void bound_explicit_inline(int a, int c) = win 0x433, ios inline {
        return 44;
    }

    // result: win inline, imac default, m1 default, ios 0x5467
    void bound_implicit_inline(int a, int c) = mac default, ios 0x5467 {
        ;;;;;
    }

    void thing(int c) = win 0x5, imac 0x8, m1 0x4, ios default;

    // result: win inline, imac inline, m1 inline, ios inline
    void normal_inline(int b) {
        return 32;
    }

    [[since("v2.10.0-alpha.1")]]
    void new_feature(int a, int b) = win 0x0, imac 0x24, m1 0x554, ios 0x343;

    int m_testBegin;

    mac, win {
        PAD = 0x5;
        int m_perPlatformTest;
    }

    PAD = mac 0x38, ios 0x984, win 0x1;
    char m_testMain;
    PAD = m1 0x53;
}