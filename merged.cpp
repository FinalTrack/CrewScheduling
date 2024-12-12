#include <bits/stdc++.h>
using namespace std;

struct edge {
    int u; //Start node
    int v; //End node
    int d; //Departure time
    int w; //Weight

    bool operator < (const edge n) const
    {
        return d < n.d;
    }
};

int64_t gen() 
{
    std::random_device rd;  
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(0, 1e18);
    return dist(gen);
}

//Convert to minutes, where monday 00:00 is 0
int get_minutes(string dayTime) {
    unordered_map<string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };

    istringstream ss(dayTime);
    string day, time;
    ss >> day >> time;
    //cout << day << " " << time << endl;
    int hours, minutes;
    if(time.length() == 5)
    {
        hours = stoi(time.substr(0, 2));
        minutes = stoi(time.substr(3, 2));
    }
    else
    {
        hours = stoi(time.substr(0, 1));
        minutes = stoi(time.substr(2, 2));
    }
    return dayToMinutes[day] + hours * 60 + minutes;
}

int SMALL;

void readFile(vector<edge>& edges, int& cnt, map<string, int>& ind)
{
    SMALL = 0;
    map<string, string> days;
    days["MO"] = "Mon";
    days["TU"] = "Tue";
    days["WE"] = "Wed";
    days["TH"] = "Thu";
    days["FR"] = "Fri";
    days["SA"] = "Sat";
    days["SU"] = "Sun";

    ifstream file("data.csv");
    string line;

    getline(file, line); //Ignore header

    while (getline(file, line)) {
        istringstream ss(line);
        //trainNo, startStation, departureTime, endStation, arrivalTime, weight
        string tstr, ustr, dstr, vstr, astr, wstr;

        //This is for data.csv
        getline(ss, tstr, ',');
        getline(ss, ustr, ',');
        getline(ss, dstr, ',');
        getline(ss, vstr, ',');
        getline(ss, astr, ',');
        getline(ss, wstr, ',');

        //This is for data2.csv
        // string dstr1, dstr2, astr1, astr2;
        // getline(ss, dstr1, ',');
        // getline(ss, dstr2, ',');
        // getline(ss, ustr, ',');
        // getline(ss, vstr, ',');
        // getline(ss, astr1, ',');
        // getline(ss, astr2, ',');

        // while(astr2.length() > 2)
        // {
        //     astr2.pop_back();
        // }
        // assert(days.find(dstr2) != days.end());
        // assert(days.find(astr2) != days.end());
        // dstr = days[dstr2] + " " + dstr1; 
        // astr = days[astr2] + " " + astr1; 


        //Assign indices to nodes
        if(ind.find(ustr) == ind.end())
            ind[ustr] = cnt++;

        if(ind.find(vstr) == ind.end()) 
            ind[vstr] = cnt++;


        int u = ind[ustr];
        int v = ind[vstr];
        int d = get_minutes(dstr);
        int a = get_minutes(astr);

        //Start on a Sunady, end on a Monday case
        if(a < d)
            a += 10080;
        int w = a - d;
        edges.push_back({u, v, d, w});

        //cout << (double)w/60 << endl;
    }
}

int calc(map<string, int> ind, vector<edge> edges, int cnt, int threshold) 
{
    set<string> homeStations = {
        "TVC", "KCVL", "NCJ", "CAPE", "ERS", 
        "QLN", "ERN", "MDU", "TEN", "SRR", 
        "PGT", "CLT", "ED"
    }; //Home stations

    set<int> homeCodes;
    for(string s: homeStations)
        homeCodes.insert(ind[s]);

    set<int> available = homeCodes; //We can start from any of these nodes
    set<int> blocked; // These edges have already been used

    vector<vector<int>> adj(cnt);

    for(int i = 0; i < edges.size(); i++)
    {
        auto [u, v, d, w] = edges[i];
        adj[u].push_back(i);
    }

    int week = 10080; //Minutes in a week
    int rest = 960; //Minutes for a rest
    int people = 0;

    //Can I return to home, from node i, with journey start st, current time t, and last home visit r
    //If so return the edge, and return t
    function<pair<int, int>(int, int, int, int, int)> can = [&](int home, int i, int st, int t, int r)
    {
        pair<int, int> ret = {INT_MAX, INT_MAX};
        for(int j: adj[i])
        {
            auto [u, v, d, w] = edges[j];
            if(blocked.find(j) != blocked.end())
                continue;
            if(v != home)
                continue;
            if(d < st)
                d += week;
            if(t > d)
                continue;
            int nt = d+w;
            if(nt - st + rest < week && t - r < rest)
            {
                ret = min(ret, {nt, j});
            }
        }
        return ret;
    };

    while(!available.empty())
    {
        //Choose a random home station which still has potential for routes
        int homeInd = gen()%(int)available.size();
        int home;
        for(int i: available)
        {
            if(homeInd)
                homeInd--;
            else
            {
                home = i;
                break;
            }
        }

        int node = home;
        int st = -1;
        int t, r;
        set<int> vis = {home};
        pair<int, int> prev;
        //cout << home << endl;
        while(true)
        {
            array<int, 6> best = {INT_MAX, 0, 0, 0, 0, 0};
            for(int j: adj[node])
            {
                auto [u, v, d, w] = edges[j];
                if(blocked.find(j) != blocked.end())
                    continue;
                if(vis.find(v) != vis.end())
                    continue;
                int nst = st;
                if(st == -1)
                {
                    nst = d;
                    t = nst;
                    r = nst;
                }
                if(d < nst)
                    d += week;
                if(t > d)
                    continue;
                pair<int, int> p = can(home, v, nst, d+w, r);
                if(p.first != INT_MAX)
                {
                    if(d+w < best[0])
                        best =  {d+w, nst, v, j, p.first, p.second};
                }
            }
            if(best[0] == INT_MAX)
            {
                if(st == -1)
                {
                    available.erase(home);
                    break;
                }
                else if(node == home)
                {
                    people++;
                    break;
                }
                else
                {
                    vis.clear();
                    vis = {home};
                    t = prev.second + rest;
                    blocked.insert(prev.first);
                    r = t;
                    node = home;
                    //cout << t << " " << node << endl;
                }
            }
            else
            {
                t = best[0];
                st = best[1];
                node = best[2];
                blocked.insert(best[3]);
                prev = {best[5], best[4]};
                vis.insert(node);
                //cout << t << " " << node << endl;
            }
        }
    }

    cout << people << endl;
    cout << (int)edges.size() - (int)blocked.size() << endl;

    return people;
}

int main()
{
    map<string, int> ind; //Map station names to indices for easier access
    int cnt = 0; //Number of nodes
    vector<edge> edges; //Edge list
    readFile(edges, cnt, ind);
    calc(ind, edges, cnt, INT_MAX);
    return 0;
}