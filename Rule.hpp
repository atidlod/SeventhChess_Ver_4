#include "Status.hpp"
#include <algorithm>
using namespace std;

class GameRule
{
public:
    //  Victory
    int Chess_Value[10] = {10000, 100, 100, 100, 40, 50, 100, 10, 100, 0};

    unsigned int Victory_Value = 1000000;

    int SnatchCon_EatChessNum;
    int Victory_EatChessNum;
    int Victory_EatCountryNum;

    // board
    int UsedPlayer;

    int NumCountry;
    int NumSpecies = 10;
    int TotalChess;
    int BoardSide;
    int BoardSize;
    /*
    int CannonNum = 3;
    int RiderNum = 8;*/
    int PasserNum = 9;

    // RandMove
    int RandMaxMove = 100;

    // PathMask
    uint512_t Row_Mask[20], Col_Mask[20], Dec_Mask[40], Inc_Mask[40],
        Max_Distance_Mask, Long_Distance_Mask[400], Medium_Distance_Mask[400], Short_Distance_Mask[400];

    int FindChess(Status &status, int XY)
    { //查詢(x,y)位置棋子
        int c, s;
        uint512_t xy = 1;
        xy = xy << XY;
        for (c = 0; c < NumCountry; c++)
            if ((status.board.Return_Country_BitBoard(c) & xy) > 1)
                break;
        if (c == NumCountry)
            return TotalChess + 1;
        for (s = 0; s < NumSpecies; s++)
            if ((status.board.Return_Species_BitBoard(s) & xy) > 1)
                return c * NumSpecies + s;
    }
    int FindControlPlayer(Status &status, int move_cs)
    {
        int move_player(0);
        for (; move_player < UsedPlayer; move_player++)
            if (status.player[move_player].ChessControl[move_cs / NumSpecies])
                break;
        return move_player;
    }
    bool CheckKing(Status &status, int K)
    {
        if ((status.board.Return_Country_BitBoard(K) & status.board.Return_Species_BitBoard(0)) > 0)
            return true;
        else
            return false;
    }
    void PickKing(Status &status, int C)
    {
        uint512_t King_BitBoard;
        King_BitBoard = status.board.Return_Country_BitBoard(C) & status.board.Return_Species_BitBoard(0);
        status.DelChess(King_BitBoard, C * NumSpecies);
    }
    bool CheckSpecialChess(int &cs)
    {
        int s(cs % NumSpecies);

        if (s == 3 || s == 9)
            return true;
        else
            return false;
    }

    uint512_t least_significant_1_bit(uint512_t x)
    {
        return x & -x;
    }
    uint512_t most_significant_1_bit(uint512_t x)
    {
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        x |= x >> 64;
        x |= x >> 128;
        x |= x >> 256;

        return (x + 1) >> 1;
    }

    uint512_t Make_Upper_Path(uint512_t Chess_Mask, uint512_t Move_Mask, bool &Able_Jump)
    {
        uint512_t path, temp;
        Chess_Mask &= Move_Mask;

        temp = least_significant_1_bit(Chess_Mask);

        if (temp > 0)
        {
            path = (temp << 1) - 1;

            if (Able_Jump)
            {
                ~temp;
                path &= temp;
                Chess_Mask &= temp;

                temp = least_significant_1_bit(Chess_Mask);

                path |= temp;
            }
            return path;
        }
        else
            return Move_Mask;
    }
    uint512_t Make_Lower_Path(uint512_t Chess_Mask, uint512_t Move_Mask, bool &Able_Jump)
    {
        uint512_t path, temp;
        Chess_Mask &= Move_Mask;

        temp = most_significant_1_bit(Chess_Mask);

        if (temp > 0)
        {
            path = Max_Distance_Mask & ~(temp - 1);

            if (Able_Jump)
            {
                ~temp;
                path &= temp;
                Chess_Mask &= temp;

                temp = most_significant_1_bit(Chess_Mask);

                path |= temp;
            }
            return path;
        }
        else
            return Move_Mask;
    }
    uint512_t Make_PathBoard(Status &status, int &xy)
    {
        uint512_t obstacle(0), chess(0);

        int cs(FindChess(status, xy));
        int s(cs % NumSpecies);
        int move_player(FindControlPlayer(status, cs));
        int i, j;

        //建構 障礙盤面(obstacle) 與 棋子盤面(chess)
        for (i = 0; i < NumCountry; i++)
        {
            if (FindControlPlayer(status, i * NumSpecies) == move_player)
                obstacle |= status.board.Return_Country_BitBoard(i);
            chess |= status.board.Return_Country_BitBoard(i);
        }
        obstacle |= status.board.Return_Species_BitBoard(PasserNum);

        chess |= obstacle;

        //建構 路徑盤面(path)
        uint512_t Distance_Mask;
        bool Able_Row, Able_Col, Able_Dec, Able_Inc, Able_Jump, Able_Rider, Able_eater;
        switch (s) //依據不同棋種，產生不同移動方式的 移動路徑盤面(path)
        {
        case 0: //將-八方無限移動
            Able_Row = true,
            Able_Col = true,
            Able_Dec = true,
            Able_Inc = true,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Max_Distance_Mask;
            break;

        case 1: //偏-正四方無限移動

            Able_Row = true,
            Able_Col = true,
            Able_Dec = false,
            Able_Inc = false,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Max_Distance_Mask;
            break;

        case 2: //裨-斜四方無限移動

            Able_Row = false,
            Able_Col = false,
            Able_Dec = true,
            Able_Inc = true,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Max_Distance_Mask;
            break;

        case 3: //砲-正四方無限移動(跳子吃)

            Able_Row = true,
            Able_Col = true,
            Able_Dec = false,
            Able_Inc = false,
            Able_Jump = true,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Max_Distance_Mask;
            break;

        case 4: //弓-八方四格移動

            Able_Row = true,
            Able_Col = true,
            Able_Dec = true,
            Able_Inc = true,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Medium_Distance_Mask[xy];
            break;

        case 5: //弩-八方五格移動

            Able_Row = true,
            Able_Col = true,
            Able_Dec = true,
            Able_Inc = true,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Long_Distance_Mask[xy];
            break;

        case 6: //刀-斜四方一格移動

            Able_Row = false,
            Able_Col = false,
            Able_Dec = true,
            Able_Inc = true,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Short_Distance_Mask[xy];
            break;
            break;

        case 7: //劍-正四方一格移動

            Able_Row = true,
            Able_Col = true,
            Able_Dec = false,
            Able_Inc = false,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = true;

            Distance_Mask = Short_Distance_Mask[xy];
            break;
            break;

        case 8: //騎-特殊八方移動

            Able_Jump = false,
            Able_Rider = true;
            Able_eater = true;

            Distance_Mask = Medium_Distance_Mask[xy];
            break;

        case 9: //行人-八方無限移動(不可吃棋)

            Able_Row = true,
            Able_Col = true,
            Able_Dec = true,
            Able_Inc = true,
            Able_Jump = false,
            Able_Rider = false;
            Able_eater = false;

            Distance_Mask = Max_Distance_Mask;
            break;

        default:
            // cout << "Create Path Error!!" << endl;
            break;
        }

        //開始建構走步棋盤
        int x(xy % BoardSide), y(xy / BoardSide);
        uint512_t Path_BitBoard(0), one(1), temp;

        if (Able_Rider)
        { //騎走步先拆成上下左右4個部分
            int new_x, new_y, Num_Dec_Mask, Num_Inc_Mask;
            uint512_t Lower_chess, Lower_part, Upper_chess, Upper_part;

            //如果第一步沒有被阻擋，再拆成兩個斜方
            if (y + 1 < BoardSide)
            { //上方為上半遞增、上半遞減
                temp = one << (xy + BoardSide);
                if ((temp & chess) == 0)
                {
                    new_x = x;
                    new_y = y + 1;
                    Num_Dec_Mask = (new_x + new_y);
                    Num_Inc_Mask = (BoardSide - 1 - new_x + new_y);

                    Lower_part = Distance_Mask & ((one << (new_x + new_y * BoardSide)) - 1);
                    Upper_part = Distance_Mask & ~((one << (new_x + new_y * BoardSide + 1)) - 1);
                    Lower_chess = chess & Lower_part;
                    Upper_chess = chess & Upper_part;

                    Path_BitBoard |= Make_Upper_Path(Upper_chess, Dec_Mask[Num_Dec_Mask], Able_Jump) &
                                     Upper_part & Dec_Mask[Num_Dec_Mask];
                    Path_BitBoard |= Make_Upper_Path(Upper_chess, Inc_Mask[Num_Inc_Mask], Able_Jump) &
                                     Upper_part & Inc_Mask[Num_Inc_Mask];
                    Path_BitBoard |= temp;
                }
            }
            if (y - 1 >= 0)
            { //下方為下半遞增、下半遞減
                temp = one << (xy - BoardSide);
                if ((temp & chess) == 0)
                {
                    new_x = x;
                    new_y = y - 1;
                    Num_Dec_Mask = (new_x + new_y);
                    Num_Inc_Mask = (BoardSide - 1 - new_x + new_y);

                    Lower_part = Distance_Mask & ((one << (new_x + new_y * BoardSide)) - 1);
                    Upper_part = Distance_Mask & ~((one << (new_x + new_y * BoardSide + 1)) - 1);
                    Lower_chess = chess & Lower_part;
                    Upper_chess = chess & Upper_part;

                    Path_BitBoard |= Make_Lower_Path(Lower_chess, Dec_Mask[Num_Dec_Mask], Able_Jump) &
                                     Lower_part & Dec_Mask[Num_Dec_Mask];
                    Path_BitBoard |= Make_Lower_Path(Lower_chess, Inc_Mask[Num_Inc_Mask], Able_Jump) &
                                     Lower_part & Inc_Mask[Num_Inc_Mask];
                    Path_BitBoard |= temp;
                }
            }
            if (x + 1 < BoardSide)
            { //左方為上半遞增、下半遞減
                temp = one << (xy + 1);
                if ((temp & chess) == 0)
                {
                    new_x = x + 1;
                    new_y = y;
                    Num_Dec_Mask = (new_x + new_y);
                    Num_Inc_Mask = (BoardSide - 1 - new_x + new_y);

                    Lower_part = Distance_Mask & ((one << (new_x + new_y * BoardSide)) - 1);
                    Upper_part = Distance_Mask & ~((one << (new_x + new_y * BoardSide + 1)) - 1);
                    Lower_chess = chess & Lower_part;
                    Upper_chess = chess & Upper_part;

                    Path_BitBoard |= Make_Lower_Path(Lower_chess, Dec_Mask[Num_Dec_Mask], Able_Jump) &
                                     Lower_part & Dec_Mask[Num_Dec_Mask];
                    Path_BitBoard |= Make_Upper_Path(Upper_chess, Inc_Mask[Num_Inc_Mask], Able_Jump) &
                                     Upper_part & Inc_Mask[Num_Inc_Mask];
                    Path_BitBoard |= temp;
                }
            }
            if (x - 1 >= 0)
            { //右方為下半遞增、上半遞減
                temp = one << (xy - 1);
                if ((temp & chess) == 0)
                {
                    new_x = x - 1;
                    new_y = y;
                    Num_Dec_Mask = (new_x + new_y);
                    Num_Inc_Mask = (BoardSide - 1 - new_x + new_y);

                    Lower_part = Distance_Mask & ((one << (new_x + new_y * BoardSide)) - 1);
                    Upper_part = Distance_Mask & ~((one << (new_x + new_y * BoardSide + 1)) - 1);
                    Lower_chess = chess & Lower_part;
                    Upper_chess = chess & Upper_part;

                    Path_BitBoard |= Make_Upper_Path(Upper_chess, Dec_Mask[Num_Dec_Mask], Able_Jump) &
                                     Upper_part & Dec_Mask[Num_Dec_Mask];
                    Path_BitBoard |= Make_Lower_Path(Lower_chess, Inc_Mask[Num_Inc_Mask], Able_Jump) &
                                     Lower_part & Inc_Mask[Num_Inc_Mask];
                    Path_BitBoard |= temp;
                }
            }
        }
        else
        { //除了騎以外的走步
            uint512_t Upper_chess, Lower_chess;
            int Num_Row_Mask(y), Num_Col_Mask(x),
                Num_Dec_Mask(x + y), Num_Inc_Mask(BoardSide - 1 - x + y);
            //根據選擇移動棋子的位置，將所有棋子拆成上下兩半部分(不包含自己)
            Lower_chess = Distance_Mask & ((one << xy) - 1);
            Upper_chess = Distance_Mask & ~((one << (xy + 1)) - 1);
            Lower_chess = chess & Lower_chess;
            Upper_chess = chess & Upper_chess;

            if (Able_Row) //水行移動
                Path_BitBoard |= Make_Upper_Path(Upper_chess, Row_Mask[Num_Row_Mask], Able_Jump) &
                                 Make_Lower_Path(Lower_chess, Row_Mask[Num_Row_Mask], Able_Jump) &
                                 Row_Mask[Num_Row_Mask];
            if (Able_Col) //垂直移動
                Path_BitBoard |= Make_Upper_Path(Upper_chess, Col_Mask[Num_Col_Mask], Able_Jump) &
                                 Make_Lower_Path(Lower_chess, Col_Mask[Num_Col_Mask], Able_Jump) &
                                 Col_Mask[Num_Col_Mask];
            if (Able_Dec) //遞減移動
                Path_BitBoard |= Make_Upper_Path(Upper_chess, Dec_Mask[Num_Dec_Mask], Able_Jump) &
                                 Make_Lower_Path(Lower_chess, Dec_Mask[Num_Dec_Mask], Able_Jump) &
                                 Dec_Mask[Num_Dec_Mask];
            if (Able_Inc) //遞增移動
                Path_BitBoard |= Make_Upper_Path(Upper_chess, Inc_Mask[Num_Inc_Mask], Able_Jump) &
                                 Make_Lower_Path(Lower_chess, Inc_Mask[Num_Inc_Mask], Able_Jump) &
                                 Inc_Mask[Num_Inc_Mask];
        }
        if (Able_eater) //如果該子有吃子能力只移除obstacle物件，否則移除所有棋子
            Path_BitBoard &= ~obstacle;
        else
            Path_BitBoard &= ~chess;

        return Path_BitBoard & Distance_Mask;
    }

    bool CheckPick(Status &status, int &XY)
    {
        return status.NowPlayer == FindControlPlayer(status, FindChess(status, XY));
    }
    bool EatStep(Status &status, int &move_cs, int &eaten_cs)
    {
        int move_player(FindControlPlayer(status, move_cs));
        int eaten_player(FindControlPlayer(status, eaten_cs));

        if (eaten_cs < TotalChess)
        { // EatChess Process
            int k_player = eaten_cs / NumSpecies;
            if (Check_SnatchControl(status, move_player, k_player, eaten_cs))
            { //////////////////////////////////////////////
                PickKing(status, k_player);

                status.SnatchControl(move_player, eaten_player);
                status.player[move_player].EatCountryNum += status.player[eaten_player].EatCountryNum + 1;
                // cout<<"Player "<<move_player+1<<" eat "<<eaten_cs<<" Get "<<i+1<<" Control"<<endl;
            }
            status.player[move_player].EatChessNum[k_player]++;
            return true;
        }
        else
            return false;
    }
    bool MoveStep(Status &status, int &Move_XY, int &Orig_XY)
    {
        int move_cs(FindChess(status, Orig_XY));
        int eaten_cs(FindChess(status, Move_XY));

        if (EatStep(status, move_cs, eaten_cs))
            status.PickChess(Move_XY, eaten_cs);

        status.MoveChess(Move_XY, Orig_XY, move_cs);
        return true;
    }
    void Make_PathMask()
    {
        int i, j;
        uint512_t one = 1, temp;
        // Mask
        // Row
        Row_Mask[0] = 0;
        for (i = 0; i < BoardSide; i++)
            Row_Mask[0] = (Row_Mask[0] << 1) + 1;
        for (i = 1; i < BoardSide; i++)
            Row_Mask[i] = Row_Mask[0] << (i * BoardSide);

        // Column
        Col_Mask[0] = 0;
        for (i = 0; i < BoardSide; i++)
            Col_Mask[0] = (Col_Mask[0] << BoardSide) + 1;
        for (i = 1; i < BoardSide; i++)
            Col_Mask[i] = Col_Mask[0] << i;

        for (i = 0; i < BoardSide * 2 - 1; i++)
        {
            Dec_Mask[i] = 0;
            Inc_Mask[i] = 0;
        }
        // Decrease ( LeftDown to RightUp )
        for (i = 0; i < BoardSize; i++)
            Dec_Mask[i % BoardSide + i / BoardSide] |= one << i;
        // Increase ( RightDown to LeftUp )
        for (i = 0; i < BoardSize; i++)
            Inc_Mask[BoardSide - 1 - i % BoardSide + i / BoardSide] |= one << i;

        // Make Distance Mask
        Max_Distance_Mask = (one << BoardSize) - 1;

        for (i = 0; i < BoardSize; i++)
        {
            temp = one << i;
            for (j = 1; j <= 5; j++)
            {
                if (i % BoardSide + j < BoardSide)
                    temp |= temp << 1;
                if (i % BoardSide - j >= 0)
                    temp |= temp >> 1;
                if (i / BoardSide + j < BoardSide)
                    temp |= temp << BoardSide;
                if (i / BoardSide - j >= 0)
                    temp |= temp >> BoardSide;

                if (j == 1)
                    Short_Distance_Mask[i] = temp;
                if (j == 4)
                    Medium_Distance_Mask[i] = temp;
                if (j == 5)
                    Long_Distance_Mask[i] = temp;
            }
        }
    }

    bool Check_SnatchControl(Status &status, int &mp, int &ep, int &cs)
    { //確認該玩家是否可獲得控制權
        if (CheckKing(status, ep))
            if (cs % NumSpecies == 0 || status.player[mp].EatChessNum[ep] + 1 == SnatchCon_EatChessNum)
                return true;
        return false;
    }
    bool Victory_ByEatCountry(Status &status, int &p, int N)
    {
        if (status.player[p].EatCountryNum + N >= Victory_EatCountryNum)
        {
            // cout<<"Eat Country Win!!"<<endl;
            return true;
        }
        return false;
    }
    bool Victory_ByEatChess(Status &status, int &p)
    {
        int Sum = 0;
        for (int i = 0; i < 7; i++)
            Sum += status.player[p].EatChessNum[i];
        if (Sum + 1 >= Victory_EatChessNum)
        {
            // cout<<"Eat Chess Win!!"<<endl;
            return true;
        }

        return false;
    }

    bool MovePossibility(Status &status)
    {
        if (status.player[status.NowPlayer].Survive)
        {
            int Orig_XY, Move_XY, move_cs;
            int l(BoardSize);
            uint512_t path;
            for (Orig_XY = 0; Orig_XY < l; Orig_XY++)
            {
                if (!CheckPick(status, Orig_XY))
                    continue;
                path = Make_PathBoard(status, Orig_XY);

                if (path > 0)
                    return true;
            }
        }
        return false;
    }
    bool Victory(Status &status)
    {
        if (status.player[status.NowPlayer].EatCountryNum >= Victory_EatCountryNum)
        {
            // cout<<"Eat Country Win!!"<<endl;
            return true;
        }

        int Sum = 0;
        for (int i = 0; i < 7; i++)
            Sum += status.player[status.NowPlayer].EatChessNum[i];
        if (Sum >= Victory_EatChessNum)
        {
            // cout<<"Eat Chess Win!!"<<endl;
            return true;
        }

        return false;
    }
    int PredictResult(Status &status)
    {
        unsigned int Victory_Value = 1000000;
        int i, j, temp;
        int l(BoardSide * BoardSide);
        int Chess_Value[10] = {10000, 100, 100, 100, 40, 50, 100, 10, 100, 0};
        int Val[UsedPlayer];

        for (j = 0; j < UsedPlayer; j++)
        {
            Val[j] = 0;
            if (!status.player[j].Survive)
                continue;
            //計算玩家盤面存活的棋子價值
            for (i = 0; i < l; i++)
            {
                temp = FindChess(status, i);
                if (temp < TotalChess)
                    if (status.player[j].ChessControl[temp])
                        Val[j] += Chess_Value[temp % NumSpecies];
            }
        }
        //計算吃棋的價值
        for (i = 0; i < UsedPlayer; i++)
        {
            temp = 0;
            for (j = 0; j < UsedPlayer; j++)
                if (CheckKing(status, i))
                    Val[j] += 1000 * status.player[j].EatChessNum[i];
        }
        //計算搶國的價值
        for (i = 0; i < UsedPlayer; i++)
            Val[i] += (Victory_Value / UsedPlayer) * status.player[i].EatCountryNum;

        for (i = 0, j = 0; i < UsedPlayer; i++)
            if (Val[i] > Val[j])
                j = i;
        return j;
    }

    ValGroup RelEvaluation(ValGroup v)
    {
        int sum(0), i;
        for (i = 0; i < 7; i++)
            sum += v.Val[i];
        //將預設勝利變數按比例分配給各玩家
        for (i = 0; i < 7; i++)
            v.RelVal[i] = Victory_Value / sum * v.Val[i];
        // v.printval();
        return v;
    }
    ValGroup BaseEvaluation(Status &status, ValGroup &Val)
    {
        int i, j, temp;
        int l(BoardSize);

        Val.Reset();
        //計算玩家盤面存活的棋子價值
        for (i = 0; i < l; i++)
        {
            temp = FindChess(status, i);

            if (temp < TotalChess)
            {
                j = FindControlPlayer(status, temp);
                Val.Val[j] += Chess_Value[temp % NumSpecies];
            }
        }

        //計算吃棋的價值
        for (i = 0; i < UsedPlayer; i++)
        {
            temp = 0;
            for (j = 0; j < UsedPlayer; j++)
                if (CheckKing(status, i))
                    Val.Val[j] += 1000 * status.player[j].EatChessNum[i];
        }
        //計算搶國的價值
        for (i = 0; i < UsedPlayer; i++)
            Val.Val[i] += (Victory_Value / UsedPlayer) * status.player[i].EatCountryNum;

        return RelEvaluation(Val);
    }
    ValGroup MoveEvaluation(Status &status, ValGroup &Val, bool &vicstep, int &orig, int &move)
    {
        ValGroup New_Val(Val);

        int move_cs(FindChess(status, orig));
        int eaten_cs(FindChess(status, move));

        int move_player(FindControlPlayer(status, move_cs));

        if (eaten_cs > TotalChess)
        { //移動目標位置是否有棋子
            if (CheckSpecialChess(move_cs))
                New_Val.RelVal[move_player] += Chess_Value[move_cs % NumSpecies];
            else
            {
                int MoveGrid(max(abs(orig % BoardSide - move % BoardSide),
                                 abs(orig / BoardSide - move / BoardSide)));
                // int MoveChess_Val(Chess_Value[move_cs % Game.board.NumSpecies]);

                // New_Val.RelVal[move_player] += MoveGrid //+ MoveChess_Val
                ;
            }
            return New_Val;
        }
        else
        {
            if (Victory_ByEatChess(status, move_player))
            { //如果該吃棋達成獲勝條件(吃棋數答標)，回傳勝利Val
                New_Val.VictoryVal(move_player, Victory_Value);
                vicstep = true;
                return New_Val;
            }

            int K_player = eaten_cs / NumSpecies;
            if (Check_SnatchControl(status, move_player, K_player, eaten_cs))
            { //吃棋動作是否產生控制權變動
                int Destroy_Num(status.player[K_player].EatCountryNum);
                if (Victory_ByEatCountry(status, move_player, Destroy_Num + 1))
                { //如果該吃棋達成獲勝條件(滅國數答標)，回傳勝利Val
                    New_Val.VictoryVal(move_player, Victory_Value);
                    vicstep = true;
                    return New_Val;
                }

                int j(0);
                for (; j < UsedPlayer; j++)
                    New_Val.Val[j] -= status.player[j].EatChessNum[K_player] * 1000;

                New_Val.Val[move_player] += New_Val.Val[K_player] - 10000 + (Victory_Value / Victory_EatCountryNum) * (Destroy_Num + 1);
                New_Val.Val[K_player] = 0;
            }
            else
            {
                int eaten_player(FindControlPlayer(status, eaten_cs));
                New_Val.Val[eaten_player] -= Chess_Value[eaten_cs % NumSpecies];
                if (CheckKing(status, K_player))
                    New_Val.Val[move_player] += 1000;
            }

            return RelEvaluation(New_Val);
        }
    }
    ValGroup RandEvaluation(Status &status, bool &vicstep, int &orig, int &move)
    {
        ValGroup New_Val;
        New_Val.Reset();

        int move_cs(FindChess(status, orig));
        int eaten_cs(FindChess(status, move));

        int move_player(FindControlPlayer(status, move_cs));

        if (eaten_cs > TotalChess)
        { //移動目標位置是否有棋子
            if (CheckSpecialChess(move_cs))
                New_Val.RelVal[move_player] += Chess_Value[move_cs % NumSpecies];

            return New_Val;
        }
        else
        {
            if (Victory_ByEatChess(status, move_player))
            { //如果該吃棋達成獲勝條件(吃棋數答標)，回傳勝利Val
                New_Val.VictoryVal(move_player, Victory_Value);
                vicstep == true;
                return New_Val;
            }

            int K_player = eaten_cs / NumSpecies;
            if (Check_SnatchControl(status, move_player, K_player, eaten_cs))
            { //吃棋動作是否產生控制權變動
                int Destroy_Num(status.player[K_player].EatCountryNum);
                if (Victory_ByEatCountry(status, move_player, Destroy_Num + 1))
                { //如果該吃棋達成獲勝條件(滅國數答標)，回傳勝利Val
                    New_Val.VictoryVal(move_player, Victory_Value);
                    vicstep == true;
                    return New_Val;
                }

                int j(0);

                New_Val.Val[move_player] += 10000;
            }
            else
            {
                New_Val.Val[move_player] += Chess_Value[eaten_cs % NumSpecies];
            }

            return New_Val;
        }
    }

    ValGroup RandMove(Status &Game, bool &V)
    {
        int GroupNum, Deep(1);
        int i, j;
        ValGroup VG[1000];
        MoveGroup MG[1000];
        uint512_t PathBoard, one(1);
        bool vicstep;
        for (vicstep = false; !vicstep && (Deep < RandMaxMove); Deep++)
        {
            GroupNum = 0;
            for (i = 0; i < BoardSize && !vicstep; i++)
            { //先對所有可行動點進行（移動Val）計算
                if (!CheckPick(Game, i))
                    continue;
                PathBoard = Make_PathBoard(Game, i);
                for (j = 0; j < BoardSize && !vicstep; j++)
                {
                    if ((PathBoard & (one << j)) == 0)
                        continue;
                    VG[GroupNum] = RandEvaluation(Game, vicstep, i, j);
                    MG[GroupNum].SetXY(i, j);
                    //如果收尋到獲勝走步,直接break
                    GroupNum++;
                }
            }

            if (vicstep)
            { //如果有勝利走步,只延伸勝利走步的節點
                break;
            }
            if (GroupNum == 0)
            { //若沒有可行動點,跳過此玩家
                Game.NowPlayer = (Game.NowPlayer + 1) % UsedPlayer;
                while (!Game.player[Game.NowPlayer].Survive)
                    Game.NowPlayer = (Game.NowPlayer + 1) % UsedPlayer;
            }
            else
            {

                for (i = 0, j = 0; i < GroupNum; i++)
                {
                    //對所有可行動點依照Val排序（大>小）
                    if (VG[0].Val[Game.NowPlayer] < VG[i].Val[Game.NowPlayer])
                    {
                        swap(VG[0], VG[i]);
                        swap(MG[0], MG[i]);
                        j = 1;
                    }
                    if (VG[0].Val[Game.NowPlayer] == VG[i].Val[Game.NowPlayer])
                    {
                        swap(VG[j], VG[i]);
                        swap(MG[j], MG[i]);
                        j++;
                    }
                }

                //返回最大價值走步
                j = rand() % j;

                swap(VG[0], VG[j]);
                swap(MG[0], MG[j]);
                {
                    MoveStep(Game, MG[0].Move_XY, MG[0].Orig_XY);
                    Game.NowPlayer = (Game.NowPlayer + 1) % UsedPlayer;
                    while (!Game.player[Game.NowPlayer].Survive)
                        Game.NowPlayer = (Game.NowPlayer + 1) % UsedPlayer;
                }
            }
        }
        if (Deep == 1)
            V = vicstep;
        if (Deep < RandMaxMove)
            return VG[GroupNum - 1];
        else
            return BaseEvaluation(Game, VG[0]);
    }
    ValGroup RandMove(Status Game, bool &V, int &Move_XY, int &Orig_XY)
    {
        MoveStep(Game, Move_XY, Orig_XY);
        Game.NowPlayer = (Game.NowPlayer + 1) % UsedPlayer;
        while (!Game.player[Game.NowPlayer].Survive)
            Game.NowPlayer = (Game.NowPlayer + 1) % UsedPlayer;
        return RandMove(Game, V);
    }

    void RuleSet_Normal(Status &status)
    { // 19格 7國 棋盤擺放
        SnatchCon_EatChessNum = 10;
        Victory_EatChessNum = 30;
        Victory_EatCountryNum = 2;
        UsedPlayer = 7;
        NumCountry = 7;
        TotalChess = NumCountry * NumSpecies;
        BoardSide = 19;
        BoardSize = BoardSide * BoardSide;

        status.StatusSet_Normal(BoardSide);

        Make_PathMask();
    }
    void RuleSet_Medium(Status &status)
    { // 11格 4國 棋盤擺放
        SnatchCon_EatChessNum = 10;
        Victory_EatChessNum = 30;
        Victory_EatCountryNum = 2;
        UsedPlayer = 4;
        NumCountry = 4;
        TotalChess = NumCountry * NumSpecies;
        BoardSide = 11;
        BoardSize = BoardSide * BoardSide;

        status.StatusSet_Medium(BoardSide);

        Make_PathMask();
    }
};