#include <bits/stdc++.h>
using namespace std;

struct edge {
    int u;
    int v;
    int d;
    int w;

    bool operator < (const edge n) const
    {
        return d < n.d;
    }
};

// Helper function to convert "Day HH:MM" to minutes from Sun 00:00
int convertToMinutes(const std::string& dayTime) {
    std::unordered_map<std::string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };

    std::istringstream ss(dayTime);
    std::string day, time;
    ss >> day >> time;
    int hours, minutes;
    if(time.length() == 5)
    {
        hours = std::stoi(time.substr(0, 2));
        minutes = std::stoi(time.substr(3, 2));
    }
    else
    {
        hours = std::stoi(time.substr(0, 1));
        minutes = std::stoi(time.substr(2, 2));
    }
    return dayToMinutes[day] + hours * 60 + minutes;
}

int main() {
    std::ifstream file("data.csv");
    std::string line;
    map<string, int> ind;
    int cnt = 0;
    vector<edge> edges;

    std::set<std::string> home = {
        "TVC", "KCVL", "NCJ", "CAPE", "ERS", 
        "QLN", "ERN", "MDU", "TEN", "SRR", 
        "PGT", "CLT", "ED"
    };

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        string tstr, ustr, dstr, vstr, astr, wstr;

        std::getline(ss, tstr, ',');
        std::getline(ss, ustr, ',');
        std::getline(ss, dstr, ',');
        std::getline(ss, vstr, ',');
        std::getline(ss, astr, ',');
        std::getline(ss, wstr, ',');

        if (ind.find(ustr) == ind.end()) {
            ind[ustr] = cnt++;
        }

        if (ind.find(vstr) == ind.end()) {
            ind[vstr] = cnt++;
        }

        int u = ind[ustr];
        int v = ind[vstr];
        int d = convertToMinutes(dstr);
        int a = convertToMinutes(astr);
        if(a < d)
            a += 10080;
        int w = a - d;
        edges.push_back({u, v, d, w});
    }

    set<int> homeCodes;
    for(string s: home)
        homeCodes.insert(ind[s]);

    cout << cnt << endl;
    sort(edges.begin(), edges.end());


    //time, home station
    vector<multiset<pair<int, int>>> wait(cnt);

    int invalid = 0;
    int people = 0;
    int stuck = 0;

    for(auto [u, v, d, w]: edges)
    {
        //cout << u << " " << v << " " << d << " " << w << endl;
        bool send = false;
        for(auto [t, i]: wait[u])
        {
            if(t > d)
                break;
            //Only send ppl back and forth from home
            if(i != u && i != v)
                continue;

            wait[u].erase(wait[u].find({t, i}));
            wait[v].insert({d+w+w, i});
            send = true;
            break;
        }
        if(!send)
        {
            if(homeCodes.find(u) != homeCodes.end())
                wait[v].insert({d+w+w, u});
            else
                invalid++;
        }
    }

    for(int i = 0; i < cnt; i++)
    {
        for(auto [t, j]: wait[i])
        {
            people++;
            if(i != j)
                stuck++;
        }
    }

    cout << people << " " << stuck << " " << invalid << endl;
    

    return 0;
}
