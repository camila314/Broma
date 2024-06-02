[[link(android)]]
class Test {
    int member(std::string str);
    int member2(std::string str);
    ///Woah

    void bound_explicit_inline(int a, int c) = win 0x433, ios inline, imac default {
        return 44;
    }

    void bound_implicit_inline(int a, int c) = imac default, ios 0x5467 {
        ;;;;;
    }

    void thing(int c) = m1 0x4, win 0x5, mac 0x8, ios default;

    void normal_inline(int b) {
        return 32;
    }

    mac, win {
        PAD = 0x5;
        int m_test;
    }

    PAD = mac 0x38, ios 0x984, win 0x1;
    char m_test;
}