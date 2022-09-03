#include "include\uintwide_t.h"
#include "Head.hpp"

using uint512_t = math::wide_integer::uint512_t;

class Board
{
private:
    uint512_t Species_BitBoard[10], Country_BitBoard[7];

public:
    void Reset()
    {
        for (int i(0); i < 10; i++)
            Species_BitBoard[i] = 0;
        for (int i(0); i < 7; i++)
            Country_BitBoard[i] = 0;
    }
    void Normal_Chess_Set(int cs, uint512_t input)
    {
        Species_BitBoard[cs % 10] = input;
        Country_BitBoard[cs / 10] = input;
    }
    void King_Chess_Set(uint512_t input)
    {
        Species_BitBoard[9] = input;
    }
    void BoardSet_Normal(int BoardSide)
    { //正常棋盤擺放
        uint512_t one = 1;
        //周天子
        King_Chess_Set(one << (9 + 9 * BoardSide));
        //將
        Normal_Chess_Set(0, one << (0 + 9 * BoardSide));
        Normal_Chess_Set(10, one << (5 + 18 * BoardSide));
        Normal_Chess_Set(20, one << (13 + 18 * BoardSide));
        Normal_Chess_Set(30, one << (18 + 13 * BoardSide));
        Normal_Chess_Set(40, one << (18 + 5 * BoardSide));
        Normal_Chess_Set(50, one << (13 + 0 * BoardSide));
        Normal_Chess_Set(60, one << (5 + 0 * BoardSide));
        //偏
        Normal_Chess_Set(1, one << (0 + 8 * BoardSide));
        Normal_Chess_Set(11, one << (4 + 18 * BoardSide));
        Normal_Chess_Set(21, one << (12 + 18 * BoardSide));
        Normal_Chess_Set(31, one << (18 + 14 * BoardSide));
        Normal_Chess_Set(41, one << (18 + 6 * BoardSide));
        Normal_Chess_Set(51, one << (14 + 0 * BoardSide));
        Normal_Chess_Set(61, one << (6 + 0 * BoardSide));
        //裨
        Normal_Chess_Set(2, one << (0 + 10 * BoardSide));
        Normal_Chess_Set(12, one << (6 + 18 * BoardSide));
        Normal_Chess_Set(22, one << (14 + 18 * BoardSide));
        Normal_Chess_Set(32, one << (18 + 12 * BoardSide));
        Normal_Chess_Set(42, one << (18 + 4 * BoardSide));
        Normal_Chess_Set(52, one << (12 + 0 * BoardSide));
        Normal_Chess_Set(62, one << (4 + 0 * BoardSide));
        //砲
        Normal_Chess_Set(3, one << (1 + 9 * BoardSide));
        Normal_Chess_Set(13, one << (5 + 17 * BoardSide));
        Normal_Chess_Set(23, one << (13 + 17 * BoardSide));
        Normal_Chess_Set(33, one << (17 + 13 * BoardSide));
        Normal_Chess_Set(43, one << (17 + 5 * BoardSide));
        Normal_Chess_Set(53, one << (13 + 1 * BoardSide));
        Normal_Chess_Set(63, one << (5 + 1 * BoardSide));
        //弓
        Normal_Chess_Set(4, one << (3 + 9 * BoardSide));
        Normal_Chess_Set(14, one << (5 + 15 * BoardSide));
        Normal_Chess_Set(24, one << (13 + 15 * BoardSide));
        Normal_Chess_Set(34, one << (15 + 13 * BoardSide));
        Normal_Chess_Set(44, one << (15 + 5 * BoardSide));
        Normal_Chess_Set(54, one << (13 + 3 * BoardSide));
        Normal_Chess_Set(64, one << (5 + 3 * BoardSide));
        //弩
        Normal_Chess_Set(5, one << (2 + 9 * BoardSide));
        Normal_Chess_Set(15, one << (5 + 16 * BoardSide));
        Normal_Chess_Set(25, one << (13 + 16 * BoardSide));
        Normal_Chess_Set(35, one << (16 + 13 * BoardSide));
        Normal_Chess_Set(45, one << (16 + 5 * BoardSide));
        Normal_Chess_Set(55, one << (13 + 2 * BoardSide));
        Normal_Chess_Set(65, one << (5 + 2 * BoardSide));
        //刀
        Normal_Chess_Set(6, one << (2 + 8 * BoardSide) | one << (2 + 10 * BoardSide));
        Normal_Chess_Set(16, one << (4 + 16 * BoardSide) | one << (6 + 16 * BoardSide));
        Normal_Chess_Set(26, one << (12 + 16 * BoardSide) | one << (14 + 16 * BoardSide));
        Normal_Chess_Set(36, one << (16 + 14 * BoardSide) | one << (16 + 12 * BoardSide));
        Normal_Chess_Set(46, one << (16 + 4 * BoardSide) | one << (16 + 6 * BoardSide));
        Normal_Chess_Set(56, one << (14 + 2 * BoardSide) | one << (12 + 2 * BoardSide));
        Normal_Chess_Set(66, one << (6 + 2 * BoardSide) | one << (4 + 2 * BoardSide));
        //劍
        Normal_Chess_Set(7, one << (0 + 7 * BoardSide) | one << (0 + 11 * BoardSide) | one << (1 + 8 * BoardSide) | one << (1 + 10 * BoardSide));
        Normal_Chess_Set(17, one << (3 + 18 * BoardSide) | one << (7 + 18 * BoardSide) | one << (4 + 17 * BoardSide) | one << (6 + 17 * BoardSide));
        Normal_Chess_Set(27, one << (11 + 18 * BoardSide) | one << (15 + 18 * BoardSide) | one << (12 + 17 * BoardSide) | one << (14 + 17 * BoardSide));
        Normal_Chess_Set(37, one << (18 + 11 * BoardSide) | one << (18 + 15 * BoardSide) | one << (17 + 12 * BoardSide) | one << (17 + 14 * BoardSide));
        Normal_Chess_Set(47, one << (18 + 3 * BoardSide) | one << (18 + 7 * BoardSide) | one << (17 + 4 * BoardSide) | one << (17 + 6 * BoardSide));
        Normal_Chess_Set(57, one << (11 + 0 * BoardSide) | one << (15 + 0 * BoardSide) | one << (12 + 1 * BoardSide) | one << (14 + 1 * BoardSide));
        Normal_Chess_Set(67, one << (3 + 0 * BoardSide) | one << (7 + 0 * BoardSide) | one << (4 + 1 * BoardSide) | one << (6 + 1 * BoardSide));

        //騎
        Normal_Chess_Set(8, one << (0 + 6 * BoardSide) | one << (0 + 12 * BoardSide) | one << (1 + 7 * BoardSide) | one << (1 + 11 * BoardSide));
        Normal_Chess_Set(18, one << (2 + 18 * BoardSide) | one << (8 + 18 * BoardSide) | one << (3 + 17 * BoardSide) | one << (7 + 17 * BoardSide));
        Normal_Chess_Set(28, one << (10 + 18 * BoardSide) | one << (16 + 18 * BoardSide) | one << (11 + 17 * BoardSide) | one << (15 + 17 * BoardSide));
        Normal_Chess_Set(38, one << (18 + 10 * BoardSide) | one << (18 + 16 * BoardSide) | one << (17 + 11 * BoardSide) | one << (17 + 15 * BoardSide));
        Normal_Chess_Set(48, one << (18 + 2 * BoardSide) | one << (18 + 8 * BoardSide) | one << (17 + 3 * BoardSide) | one << (17 + 7 * BoardSide));
        Normal_Chess_Set(58, one << (10 + 0 * BoardSide) | one << (16 + 0 * BoardSide) | one << (11 + 1 * BoardSide) | one << (15 + 1 * BoardSide));
        Normal_Chess_Set(68, one << (2 + 0 * BoardSide) | one << (8 + 0 * BoardSide) | one << (3 + 1 * BoardSide) | one << (7 + 1 * BoardSide));

        //行
        Normal_Chess_Set(9, one << (4 + 9 * BoardSide));
        Normal_Chess_Set(19, one << (5 + 14 * BoardSide));
        Normal_Chess_Set(29, one << (13 + 14 * BoardSide));
        Normal_Chess_Set(39, one << (14 + 13 * BoardSide));
        Normal_Chess_Set(49, one << (14 + 5 * BoardSide));
        Normal_Chess_Set(59, one << (13 + 4 * BoardSide));
        Normal_Chess_Set(69, one << (5 + 4 * BoardSide));
    }
    void BoardSet_Medium(int BoardSide)
    {
        uint512_t one = 1;
        //周天子
        King_Chess_Set(one << (5 + 5 * BoardSide));
        //將
        Normal_Chess_Set(0, one << (0 + 5 * BoardSide));
        Normal_Chess_Set(20, one << (10 + 5 * BoardSide));
        Normal_Chess_Set(10, one << (5 + 0 * BoardSide));
        Normal_Chess_Set(30, one << (5 + 10 * BoardSide));
        //偏
        Normal_Chess_Set(1, one << (0 + 4 * BoardSide));
        Normal_Chess_Set(21, one << (10 + 6 * BoardSide));
        Normal_Chess_Set(11, one << (6 + 0 * BoardSide));
        Normal_Chess_Set(31, one << (4 + 10 * BoardSide));
        //裨
        Normal_Chess_Set(2, one << (0 + 6 * BoardSide));
        Normal_Chess_Set(22, one << (10 + 4 * BoardSide));
        Normal_Chess_Set(12, one << (4 + 0 * BoardSide));
        Normal_Chess_Set(32, one << (6 + 10 * BoardSide));
        //砲
        Normal_Chess_Set(3, one << (1 + 5 * BoardSide));
        Normal_Chess_Set(23, one << (9 + 5 * BoardSide));
        Normal_Chess_Set(13, one << (5 + 1 * BoardSide));
        Normal_Chess_Set(33, one << (5 + 9 * BoardSide));
        //弓
        Normal_Chess_Set(4, one << (3 + 5 * BoardSide));
        Normal_Chess_Set(24, one << (7 + 5 * BoardSide));
        Normal_Chess_Set(14, one << (5 + 3 * BoardSide));
        Normal_Chess_Set(34, one << (5 + 7 * BoardSide));
        //弩
        Normal_Chess_Set(5, one << (2 + 5 * BoardSide));
        Normal_Chess_Set(25, one << (8 + 5 * BoardSide));
        Normal_Chess_Set(15, one << (5 + 2 * BoardSide));
        Normal_Chess_Set(35, one << (5 + 8 * BoardSide));
        //刀
        Normal_Chess_Set(6, one << (2 + 4 * BoardSide) | one << (2 + 6 * BoardSide));
        Normal_Chess_Set(26, one << (8 + 4 * BoardSide) | one << (8 + 6 * BoardSide));
        Normal_Chess_Set(16, one << (4 + 2 * BoardSide) | one << (6 + 2 * BoardSide));
        Normal_Chess_Set(36, one << (4 + 8 * BoardSide) | one << (6 + 8 * BoardSide));
        //劍
        Normal_Chess_Set(7, one << (0 + 7 * BoardSide) | one << (0 + 3 * BoardSide) | one << (1 + 6 * BoardSide) | one << (1 + 4 * BoardSide));
        Normal_Chess_Set(27, one << (10 + 7 * BoardSide) | one << (10 + 3 * BoardSide) | one << (9 + 6 * BoardSide) | one << (9 + 4 * BoardSide));
        Normal_Chess_Set(17, one << (7 + 0 * BoardSide) | one << (3 + 0 * BoardSide) | one << (6 + 1 * BoardSide) | one << (4 + 1 * BoardSide));
        Normal_Chess_Set(37, one << (7 + 10 * BoardSide) | one << (3 + 10 * BoardSide) | one << (6 + 9 * BoardSide) | one << (4 + 9 * BoardSide));
        //騎
        Normal_Chess_Set(8, one << (0 + 8 * BoardSide) | one << (0 + 2 * BoardSide) | one << (1 + 7 * BoardSide) | one << (1 + 3 * BoardSide));
        Normal_Chess_Set(28, one << (10 + 8 * BoardSide) | one << (10 + 2 * BoardSide) | one << (9 + 7 * BoardSide) | one << (9 + 3 * BoardSide));
        Normal_Chess_Set(18, one << (8 + 0 * BoardSide) | one << (2 + 0 * BoardSide) | one << (7 + 1 * BoardSide) | one << (3 + 1 * BoardSide));
        Normal_Chess_Set(38, one << (8 + 10 * BoardSide) | one << (2 + 10 * BoardSide) | one << (7 + 9 * BoardSide) | one << (3 + 9 * BoardSide));
        //行
        Normal_Chess_Set(9, one << (4 + 5 * BoardSide));
        Normal_Chess_Set(29, one << (6 + 5 * BoardSide));
        Normal_Chess_Set(19, one << (5 + 4 * BoardSide));
        Normal_Chess_Set(39, one << (5 + 6 * BoardSide));
    }
    uint512_t Return_Species_BitBoard(int i) { return Species_BitBoard[i]; }
    uint512_t Return_Country_BitBoard(int i) { return Country_BitBoard[i]; }
    void Add_Species_BitBoard(int s, uint512_t input) { Species_BitBoard[s] |= input; }
    void Del_Species_BitBoard(int s, uint512_t input) { Species_BitBoard[s] &= ~input; }
    void Add_Country_BitBoard(int c, uint512_t input) { Country_BitBoard[c] |= input; }
    void Del_Country_BitBoard(int c, uint512_t input) { Country_BitBoard[c] &= ~input; }
};

class Player
{
public:
    bool Survive;
    bool ChessControl[7];
    int EatChessNum[7];
    int EatCountryNum;

    void Reset()
    {
        Survive = false;
        for (int i(0); i < 7; i++)
        {
            ChessControl[i] = false;
            EatChessNum[i] = 0;
        }
        EatCountryNum = 0;
    }
};

class MoveGroup
{
public:
    int Orig_XY, Move_XY;

    void Reset()
    {
        Orig_XY = 0, Move_XY = 0;
    }
    void SetXY(int &orig, int &move)
    {
        Orig_XY = orig, Move_XY = move;
    }
    void print(int boardside)
    {
        cout << setw(3) << Orig_XY % boardside << setw(3) << Orig_XY / boardside << setw(3) << Move_XY % boardside << setw(3) << Move_XY / boardside << endl;
    }
};
class ValGroup
{
public:
    unsigned int Val[7], RelVal[7];

    void Reset()
    {
        Val[0] = 0,
        Val[1] = 0,
        Val[2] = 0,
        Val[3] = 0,
        Val[4] = 0,
        Val[5] = 0,
        Val[6] = 0;
        RelVal[0] = 0,
        RelVal[1] = 0,
        RelVal[2] = 0,
        RelVal[3] = 0,
        RelVal[4] = 0,
        RelVal[5] = 0,
        RelVal[6] = 0;
    }
    void Average(ValGroup Another[], int num)
    {
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < num; j++)
            {
                Val[i] += Another[j].Val[i];
                RelVal[i] += Another[j].RelVal[i];
            }
            Val[i] /= num;
            RelVal[i] /= num;
        }
    }
    void VictoryVal(int &Victory_Player, unsigned int &Victory_Value)
    {
        for (int p = 0; p < 7; p++)
        {
            Val[p] = 0;
            RelVal[p] = 0;
        }
        Val[Victory_Player] = Victory_Value;
        RelVal[Victory_Player] = Victory_Value;
    }
};