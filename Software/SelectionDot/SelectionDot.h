// DOT class used for selection on GUI during initialization
class Dot
{
    public:
        Dot(int x, int y, int sizeOfDot);
        void MoveDot(int x, int y);
        int _sizeOfDot; 
        int corners[4][2];
        int lastCorners[4][2];
};

int SetLedsFromDotCorner(int corners[4][2]);
int ClearLedsFromDotCorner(int corners[4][2]);