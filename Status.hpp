#include "Object.hpp"

class Status
{
public:
    // status
    Board board;
    Player player[7];

    int NowPlayer;

    void Reset()
    {
        for (int i = 0; i < 7; i++)
        {
            player[i].Reset();
        }
        board.Reset();
    }

    void PickChess(int xy, int cs)
    {
        uint512_t one = 1;
        board.Del_Species_BitBoard(cs%10,(one << xy));
        board.Del_Country_BitBoard(cs/10,(one << xy));
    }
    void PutChess(int xy, int cs)
    {
        uint512_t one = 1;
        board.Add_Species_BitBoard(cs%10,(one << xy));
        board.Add_Country_BitBoard(cs/10,(one << xy));
    }
    void DelChess(uint512_t input, int cs)
    {
        board.Add_Species_BitBoard(cs%10,input);
        board.Add_Country_BitBoard(cs/10,input);
    }
    
    void SnatchControl(int move_player, int eaten_player)
    {
        for (int i(0); i < 7; i++)
            if (player[eaten_player].ChessControl[i])
            {
                player[move_player].ChessControl[i] = true;
                player[eaten_player].ChessControl[i] = false;
            }
        player[eaten_player].Survive = false;
    }
    
    void MoveChess(int move_xy, int orig_xy, int move_cs)
    {                                                 //移動棋子(須先拾起棋子產生path並把吃棋先移除)
        PickChess(orig_xy,move_cs); //拿起棋子從原本位置
        PutChess(move_xy,move_cs); //放置棋子到移動位置
    }
    
    void StatusSet_Normal(int BoardSide)
    { // 19格 7國 棋盤擺放
        NowPlayer = 0;
        Reset();
        for (int i = 0; i < 7; i++)
        {
            player[i].ChessControl[i] = true;
            player[i].Survive = true;
        }

        board.BoardSet_Normal(BoardSide);
    }
    void StatusSet_Medium(int BoardSide)
    { // 11格 4國 棋盤擺放
        NowPlayer = 0;

        Reset();
        for (int i = 0; i < 4; i++)
        {
            player[i].ChessControl[i] = true;
            player[i].Survive = true;
        }

        board.BoardSet_Medium(BoardSide);
    }
};