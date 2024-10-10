class Color {
    public:
        Color();
        Color(unsigned int a, unsigned int b, unsigned int c);
        unsigned int getRed();
        unsigned int getGreen();
        unsigned int getBlue();

    private:
        unsigned int r, g, b;
};