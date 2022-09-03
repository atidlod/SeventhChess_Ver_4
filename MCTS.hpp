#include "SearchTree.hpp"

class mcts_node
{
private:
    Status game;
    GameRule rule;
    unsigned int GroupNum;
    mcts_node *father;
    ValGroup VG[1000];
    MoveGroup MG[1000];

public:
    ValGroup val;
    MoveGroup mg;
    mcts_node *leaf[1000];
    bool buildleaf;
    unsigned int leaf_num, visit_num;

    unsigned int UCB;
    unsigned int UCB_score(int &Main_Player, unsigned int &Total_node)
    {

        if (father)
            // UCB = val.RelVal[game.NowPlayer]/10000 + 10 * ;
            UCB = val.RelVal[Main_Player] + 10000 * sqrt(log(Total_node) / visit_num);

        return UCB;
    }

    void SetGame(Status &G, GameRule &r)
    {
        game = G;
        rule = r;
        val.Reset();
        leaf_num = 0;
        visit_num = 0;
        GroupNum = 0;
    }
    void SetRoot()
    {
        val = VG[0];
        mg = MG[0];
    }
    void MoveGame(ValGroup &VG, MoveGroup &MG)
    {
        val = VG;
        mg = MG;
        rule.MoveStep(game, MG.Move_XY, MG.Orig_XY);
        game.NowPlayer = (game.NowPlayer + 1) % rule.UsedPlayer;
        while (!game.player[game.NowPlayer].Survive)
            game.NowPlayer = (game.NowPlayer + 1) % rule.UsedPlayer;
    }
    void SkipGame(ValGroup &VG)
    {
        val = VG;
        game.NowPlayer = (game.NowPlayer + 1) % rule.UsedPlayer;
        while (!game.player[game.NowPlayer].Survive)
            game.NowPlayer = (game.NowPlayer + 1) % rule.UsedPlayer;
    }

    void GetFather(mcts_node *father_node) { father = father_node; }
    bool Expansion(mcts_node *new_node, unsigned int &MutaProb)
    {
        /*
        if (VGNum == 0)
            MoveSearch();*/

        if (GroupNum == -1)
        {
            new_node->SetGame(game, rule);
            new_node->SkipGame(val);
        }
        else
        {
            if (leaf_num != 0)
                for (int r(leaf_num + 1); r < GroupNum - 1; r++)
                {
                    if (rand() % 100 < MutaProb)
                    {
                        swap(VG[leaf_num], VG[r]);
                        swap(MG[leaf_num], MG[r]);
                    }
                    else
                        break;
                }

            new_node->SetGame(game, rule);
            new_node->MoveGame(VG[leaf_num], MG[leaf_num]);
        }
        leaf[leaf_num++] = new_node;

        if (leaf_num >= GroupNum)
            buildleaf = false;

        return true;
    }
    ValGroup Simulation(int &Main_Player, unsigned int &MaxSearchDeep, int &Search_Mode)
    {
        if (Search_Mode == 0)
            return GameSearch_Simulation(Main_Player, MaxSearchDeep);
        else
            return RandSearch_Simulation(Main_Player, MaxSearchDeep);
    }
    ValGroup GameSearch_Simulation(int &Main_Player, unsigned int &MaxSearchDeep)
    {
        GameSearchTable SearchRoot;

        SearchRoot.GameSearch(game, rule, MaxSearchDeep);
        ////

        GroupNum = SearchRoot.Return_Root_LeafNum();
        ValGroup tmp(SearchRoot.RootVal());

        if (SearchRoot.Root_Vicstep())
        {
            buildleaf = false;
            int i(0);
            VG[i] = SearchRoot.Return_Root_Leaf_VG(i);
            MG[i] = SearchRoot.Return_Root_Leaf_MG(i);
        }
        else
        {
            buildleaf = true;

            for (int i(0); i < GroupNum; i++)
            {
                VG[i] = SearchRoot.Return_Root_Leaf_VG(i);
                MG[i] = SearchRoot.Return_Root_Leaf_MG(i);
            }
        }

        SearchRoot.DeleteRoot();
        return tmp;
    }
    ValGroup RandSearch_Simulation(int &Main_Player, unsigned int &MaxSearchDeep)
    {
        RandSearchTable SearchRoot;

        SearchRoot.GameSearch(game, rule, MaxSearchDeep);
        ////

        GroupNum = SearchRoot.Return_Root_LeafNum();
        ValGroup tmp(SearchRoot.RootVal());

        if (SearchRoot.Root_Vicstep())
        {
            buildleaf = false;
            int i(0);
            VG[i] = SearchRoot.Return_Root_Leaf_VG(i);
            MG[i] = SearchRoot.Return_Root_Leaf_MG(i);
        }
        else
        {
            buildleaf = true;

            for (int i(0); i < GroupNum; i++)
            {
                VG[i] = SearchRoot.Return_Root_Leaf_VG(i);
                MG[i] = SearchRoot.Return_Root_Leaf_MG(i);
            }
        }

        SearchRoot.DeleteRoot();
        return tmp;
    }
    void Backpropagation()
    {
        visit_num++;
        if (father)
            father->Backpropagation();
    }
    void Backpropagation(ValGroup &BackVG)
    {
        visit_num++;
        if (BackVG.RelVal[game.NowPlayer] > val.RelVal[game.NowPlayer])
        {
            val = BackVG;
            if (father)
                father->Backpropagation(val);
        }
        else if (father)
            father->Backpropagation();
    }
    MoveGroup ReturnBestLeaf()
    {
        int i, j, maxval(0), MaxNum(0);

        for (i = 0; i < leaf_num; i++)
        {
            for (j = i + 1; j < leaf_num; j++)
                if (leaf[i]->val.RelVal[game.NowPlayer] < leaf[j]->val.RelVal[game.NowPlayer])
                    swap(leaf[i], leaf[j]);
            if (leaf[i]->val.RelVal[game.NowPlayer] == leaf[0]->val.RelVal[game.NowPlayer])
                MaxNum++;
        }

        if (MaxNum == 0)
            return MG[0];

        j = rand() % MaxNum;
        swap(leaf[0], leaf[j]);

        return leaf[0]->mg;
    }
    mcts_node *VisitLeaf()
    {
        int i, j, maxvisit(0), MaxNum(0);
        for (i = 0; i < leaf_num; i++)
        {
            if (leaf[i]->visit_num > maxvisit)
            {
                maxvisit = leaf[i]->visit_num;
                MaxNum = 0;
            }

            if (leaf[i]->visit_num == maxvisit)
                MaxNum++;
        }

        j = rand() % MaxNum;
        for (i = 0; i < leaf_num; i++)
            if (leaf[i]->visit_num == maxvisit)
            {
                if (j == 0)
                    break;
                else
                    j--;
            }

        return leaf[i];
    }
    // void PrintVal() { val.printxy(); }
};

class mcts_root
{
private:
    unsigned int node_num;
    mcts_node *node;
    unsigned int Mutation_Probability = 20;

public: /*
    void PrintVal()
    {
        for (int i = 0; i < node_num; i++)
        {
            cout << "Node " << i << " : ";
            node[i].PrintVal();
        }
    }*/
    int Select(int &Main_Player)
    {
        if (node_num == 1)
            if (node[0].buildleaf)
                return 0;
            else
                return -1;

        unsigned int sum(0), i;

        for (i = 0; i < node_num; i++)
        {
            if (node[i].buildleaf)
                sum += node[i].UCB_score(Main_Player, node_num);
            // cout << setw(3) << i << setw(3) << node[i].Getvisit_num() << setw(10) << node[i].UCB << endl;
        }
        if (sum == 0)
            return -1;

        int r(rand() % sum);
        for (i = 0; i < node_num; i++)
            if (node[i].buildleaf)
                if (r > node[i].UCB)
                    r -= node[i].UCB;
                else
                    break;

        // cout << "Total : " << setw(10) << sum << " Select Node " << setw(3) << i << endl;
        return i;
    }
    bool Expansion(unsigned int select_node, unsigned int &MutaProb)
    {
        if (node[select_node].Expansion(&node[node_num], MutaProb))
            node[node_num].GetFather(&node[select_node]);
        else
            return false;
        return true;
    }
    void InitMCTS(unsigned int &search_node_num)
    {
        node = new mcts_node[search_node_num];
    };
    void DeleteMCTS()
    {
        delete[] node;
    };
    MoveGroup MCTSearch(Status &G, GameRule &r, unsigned int &search_node_num, unsigned int &SearchDeep, int &Search_Mode)
    {
        ValGroup BackVG;
        node_num = 0;
        node[node_num].SetGame(G, r);
        BackVG = node[node_num].Simulation(G.NowPlayer, SearchDeep, Search_Mode);
        node[0].SetRoot();
        node[node_num++].Backpropagation(BackVG);

        unsigned int select_node;

        while (node_num < search_node_num)
        {
            select_node = Select(G.NowPlayer);
            // cout<<"Node :"<<setw(3)<<node_num<<" SelectNode :"<<setw(3)<<select_node<<endl;
            if (select_node == -1)
                break;

            if (Expansion(select_node, Mutation_Probability))
            {
                BackVG = node[node_num].Simulation(G.NowPlayer, SearchDeep, Search_Mode);
                node[node_num++].Backpropagation(BackVG);
            }
        }
        // PrintVal();
        return node[0].ReturnBestLeaf();
    }
    unsigned int HorizontalTree_Num()
    {
        return node[0].leaf_num;
    }
    mcts_node *HorizontalTree_Leaf(unsigned int &num)
    {
        return node[0].leaf[num];
    }
    mcts_node *VerticalTree_Root() { return &node[0]; }
};
