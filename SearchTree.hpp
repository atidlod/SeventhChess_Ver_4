#include "Rule.hpp"

class GameSearchTree
{
private:
    Status Game;
    GameRule rule;

public:
    unsigned int LeafNum = 0;
    GameSearchTree *Leaf[1000];
    ValGroup Val;
    MoveGroup Move;
    bool vicstep = false;

    void SetGame(Status &G, GameRule &R)
    {
        Game = G;
        rule = R;
        vicstep = false;
    }
    void MoveGame(ValGroup &VG, MoveGroup &MG)
    {
        Val = VG;
        Move = MG;
        rule.MoveStep(Game, MG.Move_XY, MG.Orig_XY);
        Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
        while (!Game.player[Game.NowPlayer].Survive)
            Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
    }
    void SkipGame(ValGroup &VG)
    {
        Val = VG;
        Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
        while (!Game.player[Game.NowPlayer].Survive)
            Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
    }

    MoveGroup Search()
    {
        int MaxNum(0), i, j;

        for (i = 0; i < LeafNum; i++)
        {
            for (j = i + 1; j < LeafNum; j++)
                if (Leaf[i]->Val.RelVal[Game.NowPlayer] < Leaf[j]->Val.RelVal[Game.NowPlayer])
                    swap(Leaf[i], Leaf[j]);
            if (Leaf[i]->Val.RelVal[Game.NowPlayer] == Leaf[0]->Val.RelVal[Game.NowPlayer])
                MaxNum++;
        }

        //返回最大價值走步
        j = rand() % MaxNum;

        swap(Leaf[0], Leaf[j]);

        Val = Leaf[0]->Val;
        return Leaf[0]->Move;
    }

    ValGroup BuildLeaf(unsigned int &Min_LeafNum, unsigned int Remain_LeafNum)
    {
        if (LeafNum == 0)
            return rule.RelEvaluation(Val);
        ;

        int l(rule.BoardSize), GroupNum(0);
        int i, j, cs;
        ValGroup VG[1000];
        MoveGroup MG[1000];
        uint512_t PathBoard, one(1);

        for (i = 0; i < l && !vicstep; i++)
        { //先對所有可行動點進行（移動Val）計算
            if (!rule.CheckPick(Game, i))
                continue;
            PathBoard = rule.Make_PathBoard(Game, i);
            for (j = 0; j < l && !vicstep; j++)
            {
                if ((PathBoard & (one << j)) == 0)
                    continue;
                VG[GroupNum] = rule.MoveEvaluation(Game, Val, vicstep, i, j);
                MG[GroupNum].SetXY(i, j);
                //如果收尋到獲勝走步,直接break
                if (!vicstep)
                    GroupNum++;
            }
        }

        if (vicstep)
        { //如果有勝利走步,只延伸勝利走步的節點
            Val = VG[GroupNum];
            return Val;
        }

        if (GroupNum == 0)
        { //若沒有可行動點,跳過此玩家
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->SkipGame(Val);
            return Leaf[0]->BuildLeaf(Min_LeafNum, Remain_LeafNum);
            ;
        }

        for (i = 0; i < GroupNum; i++)
            //對所有可行動點依照Val排序（大>小）
            for (j = i + 1; j < GroupNum; j++)
                if (VG[i].RelVal[Game.NowPlayer] < VG[j].RelVal[Game.NowPlayer])
                {
                    swap(VG[i], VG[j]);
                    swap(MG[i], MG[j]);
                }
        if (Remain_LeafNum < Min_LeafNum)
            Remain_LeafNum = Min_LeafNum;
        if (GroupNum > Remain_LeafNum) //若行動點過多,只對較高Val點作延伸
            GroupNum = Remain_LeafNum;

        int MaxVal(0), MaxNum(0);
        ValGroup MaxVG[GroupNum];

        for (i = 0; i < GroupNum; i++)
        {
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->MoveGame(VG[i], MG[i]);
            VG[i] = Leaf[0]->BuildLeaf(Min_LeafNum, Remain_LeafNum / GroupNum);

            if (VG[i].RelVal[Game.NowPlayer] > MaxVal)
            {
                MaxVal = VG[i].RelVal[Game.NowPlayer];
                MaxNum = 0;
            }
            if (VG[i].RelVal[Game.NowPlayer] == MaxVal)
            {
                MaxVG[MaxNum++] = VG[i];
            }
        }
        j = rand() % MaxNum;
        for (i = 0; i < MaxNum; i++)
            if (i == j)
                break;
        Val = MaxVG[i];
        return Val;
    }
    bool BuildTree(unsigned int &Min_LeafNum, unsigned int Remain_LeafNum)
    {
        Val = rule.BaseEvaluation(Game, Val);

        if (LeafNum == 0)
            return false;

        int l(rule.BoardSize), GroupNum(0);
        int i, j, cs;
        ValGroup VG[1000], tmpVG;
        MoveGroup MG[1000];
        uint512_t PathBoard, one(1);

        for (i = 0; i < l && !vicstep; i++)
        { //先對所有可行動點進行（移動Val）計算
            if (!rule.CheckPick(Game, i))
                continue;
            PathBoard = rule.Make_PathBoard(Game, i);
            for (j = 0; j < l && !vicstep; j++)
            {
                if ((PathBoard & (one << j)) == 0)
                    continue;
                VG[GroupNum] = rule.MoveEvaluation(Game, Val, vicstep, i, j);
                ;
                MG[GroupNum].SetXY(i, j);
                //如果收尋到獲勝走步,直接break
                if (!vicstep)
                    GroupNum++;
            }
        }

        if (vicstep)
        { //如果有勝利走步,只延伸勝利走步的節點
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->MoveGame(VG[GroupNum], MG[GroupNum]);
            LeafNum = 1;
            return true;
        }

        if (GroupNum == 0)
        { //若沒有可行動點,跳過此玩家
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->SkipGame(Val);
            // Leaf->BuildLeaf(Victory_Value, Min_LeafNum, Remain_LeafNum);
            LeafNum = -1;
            return false;
        }

        for (i = 0; i < GroupNum; i++)
            //對所有可行動點依照Val排序（大>小）
            for (j = i + 1; j < GroupNum; j++)
                if (VG[i].RelVal[Game.NowPlayer] < VG[j].RelVal[Game.NowPlayer])
                {
                    swap(VG[i], VG[j]);
                    swap(MG[i], MG[j]);
                }

        if (GroupNum > LeafNum) //若行動點過多,只對較高Val點作延伸
            GroupNum = LeafNum;
        else
            LeafNum = GroupNum;

#pragma omp parallel num_threads(10)
        {
#pragma omp for
            for (int k = 0; k < LeafNum; k++)
            {
                Leaf[k]->SetGame(Game, rule);
                Leaf[k]->MoveGame(VG[k], MG[k]);
                Leaf[k]->BuildLeaf(Min_LeafNum, Remain_LeafNum / GroupNum);
            }
        }
        return true;
    }
};

class GameSearchTable
{
private:
    unsigned int Search_RootNum = 400;
    unsigned int Search_Min_LeafNum = 10;
    unsigned int Search_Max_LeafNum = 1000;

    unsigned int SearchMaxDeep;
    GameSearchTree *root;
    GameSearchTree *node;

public:
    MoveGroup BestMove;
    ValGroup RootVal() { return root->Val; }

    void GameSearch(Status &game, GameRule &rule, unsigned int &SearchDeep)
    {
        SearchMaxDeep = SearchDeep;
        InitTree();
        root->SetGame(game, rule);
        root->BuildTree(Search_Min_LeafNum, Search_Max_LeafNum);
        BestMove = root->Search();
    }
    void InitTree()
    {
        root = new GameSearchTree;
        node = new GameSearchTree[SearchMaxDeep * Search_RootNum];

        int i(0), j(0);
        for (; i < Search_RootNum; i++, j = 0)
        {
            *((root->Leaf) + i) = (node + i * SearchMaxDeep);
            root->LeafNum++;

            // cout << i * SearchMaxDeep;
            for (j = 1; j < SearchMaxDeep; j++)
            {
                *(((node + i * SearchMaxDeep + j - 1)->Leaf) + 0) = (node + i * SearchMaxDeep + j);
                (node + i * SearchMaxDeep + j - 1)->LeafNum++;

                // cout << i * SearchMaxDeep + j;
            }
            // cout << endl;
        }
    }

    void DeleteRoot()
    {
        delete root;
        delete[] node;
    }

    unsigned int Return_Root_LeafNum() { return root->LeafNum; }
    ValGroup Return_Root_Leaf_VG(int &i) { return root->Leaf[i]->Val; }
    MoveGroup Return_Root_Leaf_MG(int &i) { return root->Leaf[i]->Move; }
    bool Root_Vicstep() { return root->vicstep; }
};

class RandSearchTree
{
private:
    Status Game;
    GameRule rule;

public:
    unsigned int LeafNum = 0;
    RandSearchTree *Leaf[1000];
    MoveGroup Move;
    ValGroup Val;
    bool vicstep = false;

    void SetGame(Status &G, GameRule &R)
    {
        Game = G;
        rule = R;
        vicstep = false;
    }
    void MoveGame(ValGroup &VG, MoveGroup &MG)
    {
        Val = VG;
        Move = MG;
        rule.MoveStep(Game, MG.Move_XY, MG.Orig_XY);
        Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
        while (!Game.player[Game.NowPlayer].Survive)
            Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
    }
    void SkipGame(ValGroup &VG)
    {
        Val = VG;
        Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
        while (!Game.player[Game.NowPlayer].Survive)
            Game.NowPlayer = (Game.NowPlayer + 1) % rule.UsedPlayer;
    }

    MoveGroup Search()
    {
        int MaxNum(0), i, j;

        for (i = 0; i < LeafNum; i++)
        {
            for (j = i + 1; j < LeafNum; j++)
                if (Leaf[i]->Val.RelVal[Game.NowPlayer] < Leaf[j]->Val.RelVal[Game.NowPlayer])
                    swap(Leaf[i], Leaf[j]);
            if (Leaf[i]->Val.RelVal[Game.NowPlayer] == Leaf[0]->Val.RelVal[Game.NowPlayer])
                MaxNum++;
        }

        //返回最大價值走步
        j = rand() % MaxNum;

        swap(Leaf[0], Leaf[j]);

        Val = Leaf[0]->Val;
        return Leaf[0]->Move;
    }

    ValGroup BuildLeaf()
    {
        if (LeafNum == 0)
        {
            Val = rule.RandMove(Game, vicstep);
            return Val;
        }

        int l(rule.BoardSize), GroupNum(0);
        int i, j;
        ValGroup VG[1000];
        MoveGroup MG[1000];
        uint512_t PathBoard, one(1);

        for (i = 0; i < l && !vicstep; i++)
        { //先對所有可行動點進行（移動Val）計算
            if (!rule.CheckPick(Game, i))
                continue;
            PathBoard = rule.Make_PathBoard(Game, i);
            for (j = 0; j < l && !vicstep; j++)
            {
                if ((PathBoard & (one << j)) == 0)
                    continue;
                VG[GroupNum] = rule.RandMove(Game, vicstep, i, j);
                ;
                MG[GroupNum].SetXY(i, j);
                //如果收尋到獲勝走步,直接break
                if (!vicstep)
                    GroupNum++;
            }
        }

        if (vicstep)
        { //如果有勝利走步,只延伸勝利走步的節點
            Val = VG[GroupNum];
            return Val;
        }

        if (GroupNum == 0)
        { //若沒有可行動點,跳過此玩家
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->SkipGame(Val);
            return Leaf[0]->BuildLeaf();
            ;
        }

        for (i = 0; i < GroupNum; i++)
            //對所有可行動點依照Val排序（大>小）
            for (j = i + 1; j < GroupNum; j++)
                if (VG[i].RelVal[Game.NowPlayer] < VG[j].RelVal[Game.NowPlayer])
                {
                    swap(VG[i], VG[j]);
                    swap(MG[i], MG[j]);
                }

        int MaxVal(0), MaxNum(0);
        ValGroup MaxVG[GroupNum];

        for (i = 0; i < GroupNum; i++)
        {
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->MoveGame(VG[i], MG[i]);
            VG[i] = Leaf[0]->BuildLeaf();

            if (VG[i].RelVal[Game.NowPlayer] > MaxVal)
            {
                MaxVal = VG[i].RelVal[Game.NowPlayer];
                MaxNum = 0;
            }
            if (VG[i].RelVal[Game.NowPlayer] == MaxVal)
            {
                MaxVG[MaxNum++] = VG[i];
            }
        }
        Val.Average(MaxVG, MaxNum);
        return Val;
    }
    bool BuildTree()
    {
        if (LeafNum == 0)
        {
            Val = rule.RandMove(Game, vicstep);
            return false;
        }

        int l(rule.BoardSize), GroupNum(0);
        int i, j;
        ValGroup VG[1000];
        MoveGroup MG[1000];
        uint512_t PathBoard, one(1);

        for (i = 0; i < l && !vicstep; i++)
        { //先對所有可行動點進行（移動Val）計算
            if (!rule.CheckPick(Game, i))
                continue;
            PathBoard = rule.Make_PathBoard(Game, i);
            for (j = 0; j < l && !vicstep; j++)
            {
                if ((PathBoard & (one << j)) == 0)
                    continue;
                VG[GroupNum] = rule.RandMove(Game, vicstep, i, j);
                ;
                MG[GroupNum].SetXY(i, j);
                //如果收尋到獲勝走步,直接break
                if (!vicstep)
                    GroupNum++;
            }
        }

        if (vicstep)
        { //如果有勝利走步,只延伸勝利走步的節點
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->MoveGame(VG[GroupNum], MG[GroupNum]);
            LeafNum = 1;
            return true;
        }

        if (GroupNum == 0)
        { //若沒有可行動點,跳過此玩家
            Leaf[0]->SetGame(Game, rule);
            Leaf[0]->SkipGame(Val);
            // Leaf->BuildLeaf(Victory_Value, Min_LeafNum, Remain_LeafNum);
            LeafNum = -1;
            return false;
        }

        for (i = 0; i < GroupNum; i++)
            //對所有可行動點依照Val排序（大>小）
            for (j = i + 1; j < GroupNum; j++)
                if (VG[i].RelVal[Game.NowPlayer] < VG[j].RelVal[Game.NowPlayer])
                {
                    swap(VG[i], VG[j]);
                    swap(MG[i], MG[j]);
                }

#pragma omp parallel num_threads(10)
        {
#pragma omp for
            for (int k = 0; k < LeafNum; k++)
            {
                Leaf[k]->SetGame(Game, rule);
                Leaf[k]->MoveGame(VG[k], MG[k]);
                Leaf[k]->BuildLeaf();
            }
        }
        return true;
    }
};

class RandSearchTable
{
private:
    unsigned int Search_RootNum = 400;

    unsigned int SearchMaxDeep;
    RandSearchTree *root;
    RandSearchTree *node;

public:
    MoveGroup BestMove;
    ValGroup RootVal() { return root->Val; }

    void GameSearch(Status &game, GameRule &rule, unsigned int &SearchDeep)
    {
        SearchMaxDeep = SearchDeep;
        InitTree();
        root->SetGame(game, rule);
        root->BuildTree();
        BestMove = root->Search();
    }
    void InitTree()
    {
        root = new RandSearchTree;
        node = new RandSearchTree[SearchMaxDeep * Search_RootNum];

        int i(0), j(0);
        for (; i < Search_RootNum; i++, j = 0)
        {
            *((root->Leaf) + i) = (node + i * SearchMaxDeep);
            root->LeafNum++;

            // cout << i * SearchMaxDeep;
            for (j = 1; j < SearchMaxDeep; j++)
            {
                *(((node + i * SearchMaxDeep + j - 1)->Leaf) + 0) = (node + i * SearchMaxDeep + j);
                (node + i * SearchMaxDeep + j - 1)->LeafNum++;

                // cout << i * SearchMaxDeep + j;
            }
            // cout << endl;
        }
    }

    void DeleteRoot()
    {
        delete root;
        delete[] node;
    }

    unsigned int Return_Root_LeafNum() { return root->LeafNum; }
    ValGroup Return_Root_Leaf_VG(int &i) { return root->Leaf[i]->Val; }
    MoveGroup Return_Root_Leaf_MG(int &i) { return root->Leaf[i]->Move; }
    bool Root_Vicstep() { return root->vicstep; }
};