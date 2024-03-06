[[link(ios)]]
class Test {
    int member(std::string str) = ios 0x3, mac 0x5;
    int member2(std::string str) = ios 0x3, mac 0x5;
    ///Woah
    void thing(int a, int c) = win 0x0;

    mac, win {
        PAD = 0x5;
        int m_test;
    }

    PAD = mac 0x38, ios 0x984, win 0x1;
    char m_test;
}