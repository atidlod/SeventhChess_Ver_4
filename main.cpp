#include "MCTS.hpp"

void Text();

void MCTSearchGame(int &, int &, int &);
void GameResult(Status &, GameRule &, double &, char[], char[]);
void AllPrint(Status &, GameRule &, char[]);
void partPrint(uint512_t, GameRule &);
void PrintControl(Status &, GameRule &);

int main(int argc, char *argv[])
{
    // Text();

    srand(time(NULL));

    int TotalLoop = 100;
    
    int Game_mode = 0;
    int Start_player = 0;

    cout << "Input(TotalLoop / Game_mode / Start_player) :";
    cin >> TotalLoop >> Game_mode >> Start_player;

    MCTSearchGame(TotalLoop, Game_mode, Start_player);

    return EXIT_SUCCESS;
};
void Text()
{
    Status game;
    GameRule rule;
    int i, x, y, xy;
    unsigned int n, m;
    bool Victory;
    MoveGroup MG;
    mcts_node *node;
    uint512_t Part;
    double start, end, time_used;

    char filename[30], boardname[30];
    sprintf(filename, "Game_T.txt");
    sprintf(boardname, "Board_T.txt");

    rule.RuleSet_Normal(game);

    // Row_Mask[20], Col_Mask[20], Dec_Mask[40], Inc_Mask[40]
    /*
    cout << "Row_Mask" << endl;
    for (i = 0; i < 20; i++)
    {
        cout << i << endl;
        partPrint(rule.Row_Mask[i], rule);
    system("pause");
    }*/
    /*
    cout << "Col_Mask" << endl;
    for (i = 0; i < 20; i++)
    {
        cout << i << endl;
        partPrint(rule.Col_Mask[i], rule);
    system("pause");
    }*/
    /*
    cout << "Dec_Mask" << endl;
    for (i = 0; i < 40; i++)
    {
        cout << i << endl;
        partPrint(rule.Dec_Mask[i], rule);
    system("pause");
    }*/
    /*
    cout << "Inc_Mask" << endl;
    for (i = 0; i < 40; i++)
    {
        cout << i << endl;
        partPrint(rule.Inc_Mask[i], rule);
    system("pause");
    }*/
    // partPrint(rule.Long_Distance_Mask[4+9*rule.BoardSide], rule);

    AllPrint(game, rule, boardname);

    cout << "Player " << game.NowPlayer + 1 << " Round!";
    PrintControl(game, rule);

    while (cin >> x >> y)
    {
        xy = x + y * rule.BoardSide;

        uint512_t one = 1;
        one = one << xy;
        partPrint(one, rule);

        cout << "Chess : " << rule.FindChess(game, xy) << endl;

        start = clock();
        Part = rule.Make_PathBoard(game, xy);
        end = clock();
        time_used = ((double)(end - start)) / CLOCKS_PER_SEC; // 計算實際花費時間

        partPrint(Part, rule);
        AllPrint(game, rule, boardname);
        cout << "Running Time : " << time_used << " S" << endl;
    }

    system("pause");
}

void MCTSearchGame(int &TotalLoop, int &Game_mode, int &Start_player)
{
    Status game;
    GameRule rule;
    int i;
    unsigned int n, m;
    bool Victory;
    MoveGroup MG;
    mcts_node *node;
    double start, end, gamestart, gameend, time_used;

    mcts_root MCTS;
    unsigned int Max_MCTS_Node_Num = 250;
    MCTS.InitMCTS(Max_MCTS_Node_Num);

    if (Game_mode == 0)
        n = 7;
    else
        n = 4;
    // 設定各玩家的SearchTreeDeep和MCTS收尋Node數量
    unsigned int Search_Deep[7], MCTS_num[7];
    int Search_Mode[7];
    for (i = 0; i < n; i++)
    {
        cout << "Player " << i + 1 << " [ Search_Deep(1-4) / Search_Mode(0/1) / MCTS_num(1-250) ] : ";
        cin >> Search_Deep[i] >> Search_Mode[i] >> MCTS_num[i];
    }

    // 產生亂數
    srand(time(NULL));

    fstream file, board;
    char filename[30], boardname[30];
    int Loop;

    for (Loop = 0; Loop < TotalLoop; Loop++)
    {
        sprintf(filename, "Game_%03d.txt", Loop + 1);
        sprintf(boardname, "Board_%03d.txt", Loop + 1);

        file.open(filename, ios::out | ios::trunc);
        board.open(boardname, ios::out | ios::trunc);
        file << " Loop : " << Loop << " / " << TotalLoop << " Start_player : " << Start_player;
        if (Game_mode == 0)
            file << " Normal-Board" << endl;
        else
            file << " Medium-Board" << endl;
        for (i = 0; i < n; i++)
            file << "Player " << i + 1 << " | Search_Deep " << setw(5) << Search_Deep[i] << " | MCTS_num " << setw(5) << MCTS_num[i] << " : " << endl;
        file.close();
        board.close();

        file.open(filename, ios::out | ios::app);
        board.open(boardname, ios::out | ios::app);

        if (Game_mode == 0)
            rule.RuleSet_Normal(game);
        else
            rule.RuleSet_Medium(game);

        game.NowPlayer = Start_player;
        Victory = false;

        int step = 1;

        gamestart = clock();
        for (i = 0; i < 10; i++)
        {
            if (rule.MovePossibility(game))
            {
                // Pick And Move
                cout << "Move : " << step << endl;
                board << "Move : " << step << endl;
                AllPrint(game, rule, boardname);
                cout << "Player " << game.NowPlayer + 1 << " Round!";
                PrintControl(game, rule);

                // MCTS
                start = clock();
                MG = MCTS.MCTSearch(game, rule, MCTS_num[i],
                                    Search_Deep[game.NowPlayer], Search_Mode[game.NowPlayer]);
                end = clock();
                // print move and time
                time_used = ((double)(end - start)) / CLOCKS_PER_SEC; // 計算實際花費時間
                cout << "Running Time : " << time_used << " S" << endl;
                MG.print(rule.BoardSide);
                { // start to record text
                    node = MCTS.VerticalTree_Root();
                    file << "Move : " << step << endl;
                    file << "Player " << game.NowPlayer + 1 << " RT : " << time_used << " S" << endl;
                    file << "Orig " << setw(3) << MG.Orig_XY << " Move " << setw(3) << MG.Move_XY << " Val: " << setw(6) << node->val.RelVal[game.NowPlayer] << endl;

                    file << "HorizontalTree: " << endl;
                    m = MCTS.HorizontalTree_Num();
                    for (n = 0; n < m; n++)
                    {
                        node = MCTS.HorizontalTree_Leaf(n);
                        file << "[O-" << setw(3) << node->mg.Orig_XY << " M-" << setw(3) << node->mg.Move_XY
                             << " V-" << setw(3) << node->visit_num << " | Val: " << setw(6) << node->val.RelVal[game.NowPlayer] << "] ";
                    }
                    file << endl
                         << "VerticalTree: " << endl;
                    node = MCTS.VerticalTree_Root();
                    while (node->leaf_num > 0)
                    {
                        node = node->VisitLeaf();
                        file << "[O-" << setw(3) << node->mg.Orig_XY << " M-" << setw(3) << node->mg.Move_XY
                             << " V-" << setw(3) << node->visit_num << " | Val: " << setw(6) << node->val.RelVal[game.NowPlayer] << "] ";
                    }
                    file << endl
                         << endl;
                }

                // Pick Step
                if (!rule.CheckPick(game, MG.Orig_XY))
                {
                    file << "Pick Error" << endl;
                    cout << "Pick Error" << endl;
                    continue;
                }

                // Move Step
                if (!rule.MoveStep(game, MG.Move_XY, MG.Orig_XY))
                {
                    file << "Move Error" << endl;
                    cout << "Move Error" << endl;
                    continue;
                }

                if (rule.Victory(game))
                    Victory = true;

                step++;
            }
            else
                cout << "Skip Player " << game.NowPlayer + 1 << endl;

            if (Victory)
                break;
            if (step >= 120)
            {
                game.NowPlayer = rule.PredictResult(game);
                break;
            }

            game.NowPlayer++;
            game.NowPlayer = game.NowPlayer % rule.UsedPlayer;
            i = 0;
            cout << endl
                 << endl;
        }

        gameend = clock();
        time_used = ((double)(gameend - gamestart)) / CLOCKS_PER_SEC; // 計算實際花費時間

        GameResult(game, rule, time_used, filename, boardname);

        file.close();
        board.close();
    }
    MCTS.DeleteMCTS();
};
void GameResult(Status &game, GameRule &rule, double &time_used, char filename[30], char boardname[30])
{
    fstream file;
    file.open(filename, ios::out | ios::app);
    AllPrint(game, rule, boardname);
    cout << "!-----------------!" << endl;
    cout << "!Player " << game.NowPlayer + 1 << " Win     !" << endl;
    cout << "!-----------------!" << endl;
    file << "!-----------------!" << endl;
    file << "!Player " << game.NowPlayer + 1 << " Win     !" << endl;
    file << "!-----------------!" << endl;

    int c(0), i(0), j;
    cout << setw(10) << "Country"
         << "|";
    file << setw(10) << "Country"
         << "|";
    for (; c < 7; c++)
    {
        cout << setw(2) << c << "|";
        file << setw(2) << c << "|";
    }

    for (; i < rule.UsedPlayer; i++)
    {
        cout << endl
             << "Player " << i + 1 << endl
             << setw(10) << "Eat"
             << "|";
        file << endl
             << "Player " << i + 1 << endl
             << setw(10) << "Eat"
             << "|";
        for (j = 0; j < 7; j++)
        {
            cout << setw(2) << game.player[i].EatChessNum[j] << "|";
            file << setw(2) << game.player[i].EatChessNum[j] << "|";
        }
        cout << endl
             << setw(10) << "Control"
             << "|";
        file << endl
             << setw(10) << "Control"
             << "|";
        for (j = 0; j < 7; j++)
            if (game.player[i].ChessControl[j])
            {
                cout << setw(2) << "O"
                     << "|";
                file << setw(2) << "O"
                     << "|";
            }
            else
            {
                cout << setw(2) << " "
                     << "|";
                file << setw(2) << " "
                     << "|";
            }
    }
    cout << " Game RunningTime : " << time_used << " S" << endl;
    file << " Game RunningTime : " << time_used << " S" << endl;
    file.close();
};

void AllPrint(Status &game, GameRule &rule, char boardname[13])
{ //一般全盤面顯示
    fstream file;
    file.open(boardname, ios::out | ios::app);

    int x, y, z;
    cout << setw(3) << ' ' << '|';
    file << setw(3) << ' ' << '|';
    for (y = 0; y < rule.BoardSide; y++)
    {
        cout << setw(3) << y;
        file << setw(3) << y;
    }
    cout << endl
         << setw(3) << '-' << '+';
    file << endl
         << setw(3) << '-' << '+';

    for (y = 0; y < rule.BoardSide; y++)
    {
        for (x = 0; x < 3; x++)
            cout << '-';
        for (x = 0; x < 3; x++)
            file << '-';
    }
    cout << endl;
    file << endl;

    int PrintBoard[400];
    for (x = 0; x < rule.BoardSize; x++)
        PrintBoard[x] = rule.FindChess(game, x);

    for (y = 0; y < rule.BoardSide; y++)
    {
        cout << setw(3) << y << '|';
        file << setw(3) << y << '|';

        for (x = 0; x < rule.BoardSide; x++)
        {
            z = PrintBoard[x + y * rule.BoardSide];
            if (z < rule.TotalChess)
            {
                cout << setw(3) << z;
                file << setw(3) << z;
            }
            else if (z == rule.TotalChess )
            {
                cout << setw(3) << 'K';
                file << setw(3) << 'K';
            }
            else
            {
                cout << setw(3) << '-';
                file << setw(3) << '-';
            }
        }
        cout << endl;
        file << endl;
    }

    for (y = 0; y <= (rule.BoardSide + 1) * 3; y++)
    {
        cout << '-';
        file << '-';
    }
    cout << endl;
    file << endl;

    file.close();
};

void partPrint(uint512_t part, GameRule &rule)
{ //一般全盤面顯示

    int x, y, z, xy;
    cout << setw(3) << ' ' << '|';
    for (y = 0; y < rule.BoardSide; y++)
    {
        cout << setw(3) << y;
    }
    cout << endl
         << setw(3) << '-' << '+';

    for (y = 0; y < rule.BoardSide; y++)
    {
        for (x = 0; x < 3; x++)
            cout << '-';
    }
    cout << endl;

    uint512_t temp = 1;

    for (y = 0; y < rule.BoardSide; y++)
    {
        cout << setw(3) << y << '|';

        for (x = 0; x < rule.BoardSide; x++)
        {
            if ((part & temp) > 0)
            {
                cout << setw(3) << 'O';
            }
            else
            {
                cout << setw(3) << '-';
            }
            temp = temp << 1;
        }
        cout << endl;
    }

    for (y = 0; y <= (rule.BoardSide + 1) * 3; y++)
    {
        cout << '-';
    }
    cout << endl;
};

void PrintControl(Status &game, GameRule &rule)
{
    cout << "Control:";
    for (int c = 0; c < rule.UsedPlayer; c++)
        if (game.player[game.NowPlayer].ChessControl[c])
            cout << setw(2) << c << " ";
        else
            cout << setw(3) << " ";
    cout << endl;
};
