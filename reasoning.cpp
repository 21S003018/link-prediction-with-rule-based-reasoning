#include <iostream>
#include <bits/stdc++.h>
#include <string>
#define inf 1e6
#define bottom_epsilon 0.001
#define top_epsilon 0.01
#define time_epsilon 20
#define LL long long
#define cl(x) clock_t x = clock()
using namespace std;

char* trans = "E";
char buf[1000];
int xbuf;
int times;

typedef struct hr{
    int h;
    int r;
    hr(int  x,int y):h(x),r(y){}
    bool operator<(const hr&a)const{
        if (a.h == h) return a.r < r;
        return a.h < h;
    }
}hr;
typedef struct rt{
    int r;
    int t;
    rt(int x,int y):r(x),t(y){}
    bool operator<(const rt&a)const{
        if(a.r == r) return a.t < t;
        return a.r < r;
    }
}rt;


typedef struct triplet{///triplet.
    int h;
    int r;
    int t;
    double s = 0;///score of the reasoning path, but not this triplet
    triplet(int a,int b,int c):h(a),r(b),t(c){};

	bool operator<(const triplet& a)const
	{
		if (a.h == h && a.r == r) return a.t < t;
		if (a.h == h) return a.r < r;
		return a.h < h;
	}
    bool operator==(const triplet& tmp) const{
        return this->h == tmp.h && this->r == tmp.r && this->t == tmp.t;
    }
}trip;

class MyHashFunction {
public:
    size_t operator()(const triplet& tmp) const
    {
        return (LL)tmp.h * 1000000000 + (LL)tmp.r * 100000 + (LL)tmp.t;
    }
};

typedef struct triplet_set{///triplet set corresponds to a search node.if all the node can be met, then the node can return.
    set<trip> trips;
    double s = 1;
    int last_rule = -1;
    bool operator<(const triplet_set &a)const{
        return a.s < s;
    }
    void add(int h,int r,int t){
        trips.insert(trip{h,r,t});
    }
    void add(int h,int r,int t,double s){
        trip to_be_inserted(h,r,t);
        to_be_inserted.s = s;
        trips.insert(to_be_inserted);
    }
    void filter(int h,int r,int t){
        trips.erase(trip(h,r,t));
    }
    triplet_set union_(triplet_set tmp){
        triplet_set ret;
        ret.trips.insert(trips.begin(),trips.end());
        ret.trips.insert(tmp.trips.begin(),tmp.trips.end());
        return ret;
    }
    vector<trip> get_all_trips(){
        vector<trip> ret;
        set<trip>::iterator iter = trips.begin();
        while(iter!=trips.end()){
            trip tmp = *iter;
            trip to_be_pushed = trip(tmp.h,tmp.r,tmp.t);
            to_be_pushed.s = tmp.s;
            ret.push_back(to_be_pushed);
            iter ++;
        }
        return ret;
    }
}trip_set;


typedef struct rule_unit{
    int relation;
    char x_name;
    int x_value = -1;
    char y_name;
    int y_value = -1;
    rule_unit(){}
    rule_unit(int x,char y,char z):relation(x),x_name(y),y_name(z){}
}unit;
typedef struct rule{
    int id;
    vector<unit> lefts;
    unit right;
    double s;
    rule(){}
    rule(int x,vector<unit> y,unit z):id(x),lefts(y),right(z){}
}rule;

vector<vector<double> > id2relationvec;
vector<vector<double> > id2entityvec;
vector<vector<double> > id2normalvec;
vector<vector<vector<double> > > id2transmatrix;
vector<vector<rule> > relation2rule;
unordered_set<trip,MyHashFunction> knowledgebase;
unordered_set<trip,MyHashFunction> testbase;
vector<trip> testset;
map<hr,vector<int> > hr2t;
map<rt,vector<int> > rt2h;
int entities;
int relations;


double manhattan(vector<double> vec){
    double ans = 0;
    for (int i = 0;i < vec.size();i ++){
        ans += abs(vec[i]);
    }
    return ans / vec.size();
}
vector<char*> split(char *str,const char* split){
    vector<char*> ret;
    char*p = strtok(str,split);
    while(p != NULL){
        ret.push_back(p);
        p = strtok(NULL,split);
    }
    return ret;
}
int string2int(char *str){
    return stoi(str,0,10);
}
double string2double(char*str){
    return strtod((const char*)str,NULL);
}
char* stringcopy(char*x){
    char*des = new char[strlen(x) + 1];
    strcpy(des,x);
    return des;
}
char* str_cat(const char* a,const char* b,const char*c){
    static char ans[20] = {0};
    for(int i = 0;i < strlen(ans);i ++) ans[i] = '\0';
    strcat(ans,a);
    strcat(ans,b);
    strcat(ans,c);
    return ans;
}
double transE_score(int h,int r,int t){
    double ans = 0;
    for(int i = 0;i < 100;i ++){
        ans += abs(id2entityvec[h][i] + id2relationvec[r][i] - id2entityvec[t][i]);
    }
    return ans / 100;
}
double transH_score(int h,int r,int t){
    int n = id2entityvec[0].size();
	double sum=0;
	vector<double> htop, ltop;
	vector<double> wh, wl;
	htop.resize(n);
	ltop.resize(n);
	wh.resize(n);
	wl.resize(n);
	double wTh = 0;
	double wTt = 0;

	for (int ii=0; ii<n; ii++)
		wTh+=id2normalvec[r][ii] * id2entityvec[h][ii]; //wT * h
	for (int ii=0; ii<n; ii++)
		wh[ii]=wTh * id2normalvec[r][ii]; // wT * h * w
	for (int ii=0; ii<n; ii++)
		htop[ii]=id2entityvec[h][ii] - wh[ii];// h - wT * h * w

	for (int ii=0; ii<n; ii++)
		wTt+=id2normalvec[r][ii] * id2entityvec[t][ii]; //wT * t
	for (int ii=0; ii<n; ii++)
		wl[ii]=wTt * id2normalvec[r][ii]; // wT * t * w
	for (int ii=0; ii<n; ii++)
		ltop[ii]=id2entityvec[t][ii] - wl[ii];// t - wT * t * w

	double ans = 0;
	for(int i = 0;i < 100;i ++){
        ans+=abs(htop[i] - ltop[i] + id2relationvec[r][i]);
	}
	return ans/100;
}
double transR_score(int h,int r,int t){
    int m = 100;
    int n = 100;
    vector<double> h_vec;
    h_vec.resize(m);
    vector<double> t_vec;
    t_vec.resize(m);
    cl(s);
    for (int ii=0; ii<m; ii++)
        for (int jj=0; jj<n; jj++)
        {
            h_vec[ii]+=id2transmatrix[r][jj][ii]*id2entityvec[h][jj];
            t_vec[ii]+=id2transmatrix[r][jj][ii]*id2entityvec[t][jj];
        }
    double sum=0;
    for (int ii=0; ii<m; ii++)
        sum += fabs(t_vec[ii]-h_vec[ii]-id2relationvec[r][ii]);
    return sum / 100;
}
double trans_score(int h,int r,int t){
    if(trans == "E") return transE_score(h,r,t);
    if(trans == "H") return transH_score(h,r,t);
    if(trans == "R") return transR_score(h,r,t);
}

///rule's black box
struct rule_box{
    vector<rule> rules;
    vector<vector<trip_set> > input;
    int layer = -1;
    vector<trip_set> combinations;
    void init(){
        ///read rule into memory
        int index = 0;
        FILE* rulefile = fopen(str_cat("trans",trans,"/newrule.txt"),"r");
        while(fscanf(rulefile,"%s",buf) == 1){
            rule tmp_rule = rule();
            tmp_rule.id = index++;
            vector<char*>rule_split = split(stringcopy(buf),";");
            ///calculate the Antecedent
            char* tmp = stringcopy(rule_split[0]);
            vector<char*> lefts = split(stringcopy(tmp),"|");
            for(int i = 0;i < lefts.size();i ++){
                char* temp = stringcopy(lefts[i]);
                vector<char*> ret = split(temp,",");
                tmp_rule.lefts.push_back(unit(string2int(ret[1]),ret[0][0],ret[2][0]));
            }
            ///After
            tmp = rule_split[1];
            vector<char*> ret = split(stringcopy(tmp),",");
            tmp_rule.right = unit(string2int(ret[1]),ret[0][0],ret[2][0]);
            tmp_rule.s = string2double(rule_split[2]);
            ///add
            rules.push_back(tmp_rule);
        }
        fclose(rulefile);
        ///aggregate rule with relation
		for(int i = 0;i < relations;i ++){
			vector<rule> tmp;
			relation2rule.push_back(tmp);
		}
        for (int i = 0;i < rules.size();i ++){
            relation2rule[rules[i].right.relation].push_back(rules[i]);
        }
        cout << "rules prepared!\n";
    }


    vector<trip_set> get_one_extension_of_one_node(trip tmp,double s){
        vector<trip_set> ret;
        vector<rule> tmp_rules = relation2rule[tmp.r];
        for(int i = 0;i < tmp_rules.size();i ++){
            rule tmp_rule = tmp_rules[i];
            map<char,int>mat;
            mat[tmp_rule.right.x_name] = tmp.h;
            mat[tmp_rule.right.y_name] = tmp.t;
            if(tmp_rule.lefts.size() == 1){
                trip_set sset;sset.s = s * exp(tmp_rule.s);sset.last_rule=tmp_rule.id;
                sset.add(mat[tmp_rule.lefts[0].x_name],tmp_rule.lefts[0].relation,mat[tmp_rule.lefts[0].y_name],tmp_rule.s + tmp.s);
                ret.push_back(sset);
            }else if(tmp_rule.lefts.size() == 2){
                if(tmp_rule.lefts[0].x_name == 'f'){
                    vector<int> hs = rt2h[rt(tmp_rule.lefts[0].relation,mat[tmp_rule.lefts[0].y_name])];
                    for (int j = 0;j < hs.size();j ++){
                        mat['f'] = hs[j];
                        trip_set sset;sset.s = s * exp(tmp_rule.s);sset.last_rule=tmp_rule.id;
                        sset.add(mat[tmp_rule.lefts[0].x_name],tmp_rule.lefts[0].relation,mat[tmp_rule.lefts[0].y_name],tmp_rule.s + tmp.s);
                        sset.add(mat[tmp_rule.lefts[1].x_name],tmp_rule.lefts[1].relation,mat[tmp_rule.lefts[1].y_name],tmp_rule.s + tmp.s);
                        ret.push_back(sset);
                    }
                }else{
                    vector<int> ts = hr2t[hr(tmp_rule.lefts[0].x_name,mat[tmp_rule.lefts[0].relation])];
                    for (int j = 0;j < ts.size();j ++){
                        mat['f'] = ts[j];
                        trip_set sset;sset.s = s * exp(tmp_rule.s);sset.last_rule=tmp_rule.id;
                        sset.add(mat[tmp_rule.lefts[0].x_name],tmp_rule.lefts[0].relation,mat[tmp_rule.lefts[0].y_name],tmp_rule.s + tmp.s);
                        sset.add(mat[tmp_rule.lefts[1].x_name],tmp_rule.lefts[1].relation,mat[tmp_rule.lefts[1].y_name],tmp_rule.s + tmp.s);
                        ret.push_back(sset);
                    }
                }
                if(tmp_rule.lefts[1].x_name == 'f'){
                    vector<int> hs = rt2h[rt(tmp_rule.lefts[1].relation,mat[tmp_rule.lefts[1].y_name])];
                    for (int j = 0;j < hs.size();j ++){
                        mat['f'] = hs[j];
                        trip_set sset;sset.s = s * exp(tmp_rule.s);sset.last_rule=tmp_rule.id;
                        sset.add(mat[tmp_rule.lefts[0].x_name],tmp_rule.lefts[0].relation,mat[tmp_rule.lefts[0].y_name],tmp_rule.s + tmp.s);
                        sset.add(mat[tmp_rule.lefts[1].x_name],tmp_rule.lefts[1].relation,mat[tmp_rule.lefts[1].y_name],tmp_rule.s + tmp.s);
                        ret.push_back(sset);
                    }
                }else{
                    vector<int> ts = hr2t[hr(tmp_rule.lefts[1].x_name,mat[tmp_rule.lefts[1].relation])];
                    for (int j = 0;j < ts.size();j ++){
                        mat['f'] = ts[j];
                        trip_set sset;sset.s = s * exp(tmp_rule.s);sset.last_rule=tmp_rule.id;
                        sset.add(mat[tmp_rule.lefts[0].x_name],tmp_rule.lefts[0].relation,mat[tmp_rule.lefts[0].y_name],tmp_rule.s + tmp.s);
                        sset.add(mat[tmp_rule.lefts[1].x_name],tmp_rule.lefts[1].relation,mat[tmp_rule.lefts[1].y_name],tmp_rule.s + tmp.s);
                        ret.push_back(sset);
                    }
                }
            }
        }
        return ret;
    }

    vector<trip_set> extend(trip_set tmp){
        combinations.clear();
        input.clear();
        vector<trip> all_trips = tmp.get_all_trips();
        trip to_be_extended = all_trips[0];
        return get_one_extension_of_one_node(to_be_extended,tmp.s);
    }
}rule_box;
void prepare(){
    ///read embedding of relation into memory
    FILE* relationfile = fopen(str_cat("trans",trans,"/relation2vec.txt"),"r");
    xbuf = 0;
    while (fscanf(relationfile,"%[^\n]%*c",buf)==1){
        vector<char*> doubles = split(stringcopy(buf),"\t");
        vector<double> tmp;
        double x;
        for(int i = 0;i < doubles.size();i ++){
            tmp.push_back(string2double(doubles[i]));
            x = string2double(doubles[i]);
        }
		xbuf++;
        id2relationvec.push_back(tmp);
    }
	relations = xbuf;
    fclose(relationfile);
    cout << "relation prepared!\n";
    ///read embedding of entity into memory
    FILE* entityfile = fopen(str_cat("trans",trans,"/entity2vec.txt"),"r");
    xbuf = 0;
    while (fscanf(entityfile,"%[^\n]%*c",buf)==1){
        vector<char*> doubles = split(stringcopy(buf),"\t");
        vector<double> tmp;
        for(int i = 0;i < doubles.size();i ++){
            tmp.push_back(string2double(doubles[i]));
        }
		xbuf ++;
        id2entityvec.push_back(tmp);
    }
	entities = xbuf;
    fclose(entityfile);
    cout << "entity prepared!\n";
    ///read normal vector of flat if trans is H
    if(trans == "H"){
        FILE* normal_vec_file = fopen(str_cat("trans",trans,"/A.txt"),"r");
        xbuf = 0;
        char* tmpbuf;
        while (fscanf(normal_vec_file,"%[^\n]%*c",buf)==1){
            vector<char*> doubles = split(stringcopy(buf),"\t");
            vector<double> tmp;
            for(int i = 0;i < doubles.size();i ++){
                tmpbuf = doubles[i];
                tmp.push_back(string2double(doubles[i]));
            }
            id2normalvec.push_back(tmp);
			xbuf++;
        }
        fclose(normal_vec_file);
        cout << "normal vector prepared!\n";
    }
    if(trans == "R"){
        int m = 100;
        int n = 100;
        FILE* trans_matrix_file = fopen(str_cat("trans",trans,"/A.txt"),"r");
        for (int i=0; i<relations; i++)
        {
//		    id2transmatrix[i].resize(n);
		    vector<vector<double> > tmp;
		    for (int jj=0; jj<n; jj++)
		    {
		        vector<double> ttmp;
		        ttmp.resize(m);
		        for (int ii=0; ii<m; ii++)
                    fscanf(trans_matrix_file,"%lf",&ttmp[ii]);
                tmp.push_back(ttmp);
		    }
		    id2transmatrix.push_back(tmp);
		}
		fclose(trans_matrix_file);
		cout << "trans matrix prepared!\n";
    }
    ///read knowledge base into memory
    ///aggregate knowledge base with hr and rt respectively
    FILE* f_kb1 = fopen("knowledgebase.txt","r");
	while (fscanf(f_kb1,"%s",buf)==1){
	    vector<char*> ret = split(stringcopy(buf),",");
	    int h = string2int(ret[0]);
	    int r = string2int(ret[1]);
	    int t = string2int(ret[2]);
        knowledgebase.insert(trip(h,r,t));
        hr2t[hr(h,r)].push_back(t);
        rt2h[rt(r,t)].push_back(h);
    }
    cout << "knowledge base prepared!" << endl;
    fclose(f_kb1);
    ///read test set into memory
    FILE* f_test = fopen("newtest.txt","r");
	while(fscanf(f_test,"%s",buf)==1){
	    vector<char*> ret = split(stringcopy(buf),",");
	    int h = string2int(ret[0]);
	    int r = string2int(ret[1]);
	    int t = string2int(ret[2]);
        testset.push_back(trip(h,r,t));
        testbase.insert(trip(h,r,t));
    }
    cout << "test set prepared!" << endl;
    fclose(f_test);
	rule_box.init();
}

double reasoning(int h,int r,int t){
    priority_queue<trip_set> Q;
    trip_set first_node;
    first_node.s = 1;
    first_node.add(h,r,t);
    Q.push(first_node);
    double score = 1 + trans_score(h,r,t);
    double last_version_score = score;
    times = 0;

    int testnum = 0;
    while(!Q.empty()){
        trip_set tmp_node = Q.top();Q.pop();
        if(tmp_node.s < score){
            vector<trip> all_trips = tmp_node.get_all_trips();
            for (int i = 0;i < all_trips.size();i ++){
                if(knowledgebase.count(all_trips[i])){
                    tmp_node.filter(all_trips[i].h,all_trips[i].r,all_trips[i].t);
                    tmp_node.s *= (0 + 1);
//                    cout << tmp_node.last_rule << "," << tmp_node.s << endl;
                    continue;
                }
                if (relation2rule[all_trips[i].r].size() == 0){
                    tmp_node.filter(all_trips[i].h,all_trips[i].r,all_trips[i].t);
                    double hrt_score = trans_score(all_trips[i].h,all_trips[i].r,all_trips[i].t);
                    tmp_node.s *= (hrt_score + 1);
                    continue;
                }
            }
            if (tmp_node.trips.empty()){
                if (score > tmp_node.s) score = tmp_node.s;
            }else{
                vector<trip_set> next_nodes = rule_box.extend(tmp_node);
                int n = next_nodes.size();
                for(int i = 0;i < next_nodes.size();i ++){
                    Q.push(next_nodes[i]);
                }
            }
        }
        if (score == last_version_score){
            times++;
            if (times >= time_epsilon) {
                break;
            }
        }else{
            times = 1;
            last_version_score = score;
        }
    }
    return score;
}


int tihuan;
int use_trans_l(int h,int r,int t){
    double tmptest = trans_score(h,r,t);
    int sum = 0;
    for(int i = 0;i < entities;i ++){
        if(knowledgebase.count(trip(i,r,t)) || testbase.count(trip(i,r,t))) continue;
        if(trans_score(i,r,t) < tmptest) {sum ++;}
    }
    return sum;
}
int use_trans_r(int h,int r,int t){
    double tmptest = trans_score(h,r,t);
    int sum = 0;
    for(int i = 0;i < entities;i ++){
        if(knowledgebase.count(trip(h,r,i)) || testbase.count(trip(h,r,i))) continue;
        if(trans_score(h,r,i) < tmptest) {sum ++;}
    }
    return sum;
}
void formal_experiment(){
    int sum = 0;
    double mrr_sum = 0;
    int hit_at_1_sum = 0;
    int hit_at_3_sum = 0;
    int hit_at_5_sum = 0;
    int hit_at_10_sum = 0;

    int sum_trans = 0;
    double mrr_sum_trans = 0;
    int hit_at_1_sum_trans = 0;
    int hit_at_3_sum_trans = 0;
    int hit_at_5_sum_trans = 0;
    int hit_at_10_sum_trans = 0;

    int test_num = testset.size();
//    for(int j = string2int(pid) * 10000;j < min((string2int(pid) + 1) * 10000,test_num);j ++){
    for(int j = 0;j < test_num;j ++){
        //reasoning
        cout << j << endl;
        sum += 2;
        int h = testset[j].h;
        int r = testset[j].r;
        int t = testset[j].t;
        double tmptest = reasoning(h,r,t);
        //trans
        int l = use_trans_l(h,r,t); int rr = use_trans_r(h,r,t);
        sum_trans += (l + rr + 2);
        mrr_sum_trans = mrr_sum_trans + 1.0 / (l + 1) + 1.0 / (rr + 1);
        if(l < 10) hit_at_10_sum_trans += 1;
        if(l < 5) hit_at_5_sum_trans += 1;
        if(l < 3) hit_at_3_sum_trans += 1;
        if(l < 1) hit_at_1_sum_trans += 1;

        if(rr < 10) hit_at_10_sum_trans += 1;
        if(rr < 5) hit_at_5_sum_trans += 1;
        if(rr < 3) hit_at_3_sum_trans += 1;
        if(rr < 1) hit_at_1_sum_trans += 1;

        if(tmptest - 1 >= trans_score(h,r,t) - 0.00001) {
            int tmp_l = use_trans_l(h,r,t); int tmp_r = use_trans_r(h,r,t);
            sum = sum + tmp_l + tmp_r;
            mrr_sum = mrr_sum + 1.0 / (tmp_l + 1) + 1.0 / (tmp_r + 1);
            if(tmp_l < 10) hit_at_10_sum += 1;
            if(tmp_l < 5) hit_at_5_sum += 1;
            if(tmp_l < 3) hit_at_3_sum += 1;
            if(tmp_l < 1) hit_at_1_sum += 1;

            if(tmp_r < 10) hit_at_10_sum += 1;
            if(tmp_r < 5) hit_at_5_sum += 1;
            if(tmp_r < 3) hit_at_3_sum += 1;
            if(tmp_r < 1) hit_at_1_sum += 1;
            continue;
        }
        cout << tmptest << "," << trans_score(h,r,t) << endl;
        double tmp = 1;
        for(int i = 0;i < entities;i ++){
            if(knowledgebase.count(trip(i,r,t)) || testbase.count(trip(i,r,t))) {continue;}
            double tmps = reasoning(i,r,t);
            if(tmps < tmptest) {sum ++;tmp = tmp + 1;}
        }
        cout << "L:" << tmp << "," <<use_trans_l(h,r,t) + 1 << endl;

        mrr_sum += 1 / tmp;
        if (tmp <= 10) hit_at_10_sum += 1;
        if (tmp <= 5) hit_at_5_sum += 1;
        if (tmp <= 3) hit_at_3_sum += 1;
        if (tmp <= 1) hit_at_1_sum += 1;
        tmp = 1;
        for(int i = 0;i < entities;i ++){
            if(knowledgebase.count(trip(h,r,i)) || testbase.count(trip(h,r,i))) {continue;}
            double tmps = reasoning(h,r,i);
            if(tmps < tmptest) {sum ++;tmp = tmp + 1;}
        }
        cout << "R:" << tmp << "," <<use_trans_r(h,r,t) + 1 << endl;
        mrr_sum += 1 / tmp;
        if (tmp <= 10) hit_at_10_sum += 1;
        if (tmp <= 5) hit_at_5_sum += 1;
        if (tmp <= 3) hit_at_3_sum += 1;
        if (tmp <= 1) hit_at_1_sum += 1;

    }

    FILE* ansfile = fopen("ans.txt","w");
    fprintf(ansfile,"reasoning:sum of rank, sum of reciprocal ranking,sum of hit@1,sum of hit@3,sum of hit@5,sum of hit@10:%d,%f,%d,%d,%d,%d\n",sum,mrr_sum,hit_at_1_sum,hit_at_3_sum,hit_at_5_sum,hit_at_10_sum);
    fprintf(ansfile,"trans:sum of rank, sum of reciprocal ranking,sum of hit@1,sum of hit@3,sum of hit@5,sum of hit@10:%d,%f,%d,%d,%d,%d\n",sum_trans,mrr_sum_trans,hit_at_1_sum_trans,hit_at_3_sum_trans,hit_at_5_sum_trans,hit_at_10_sum_trans);
    fclose(ansfile);
    return;
}
void test_experiment(){
    int sum = 0;
    double mrr_sum = 0;
    int hit_at_1_sum = 0;
    int hit_at_3_sum = 0;
    int hit_at_5_sum = 0;
    int hit_at_10_sum = 0;

    int sum_trans = 0;
    double mrr_sum_trans = 0;
    int hit_at_1_sum_trans = 0;
    int hit_at_3_sum_trans = 0;
    int hit_at_5_sum_trans = 0;
    int hit_at_10_sum_trans = 0;

    int test_num = testset.size();
//    for(int j = string2int(pid) * 10000;j < min((string2int(pid) + 1) * 10000,test_num);j ++){
    for(int j = 0;j < 100;j ++){
        //reasoning
        cout << j << endl;
        sum += 2;
        int h = testset[j].h;
        int r = testset[j].r;
        int t = testset[j].t;
        double tmptest = reasoning(h,r,t);
        //trans
        int l = use_trans_l(h,r,t); int rr = use_trans_r(h,r,t);
        sum_trans += (l + rr + 2);
        mrr_sum_trans = mrr_sum_trans + 1.0 / (l + 1) + 1.0 / (rr + 1);
        if(l < 10) hit_at_10_sum_trans += 1;
        if(l < 5) hit_at_5_sum_trans += 1;
        if(l < 3) hit_at_3_sum_trans += 1;
        if(l < 1) hit_at_1_sum_trans += 1;

        if(rr < 10) hit_at_10_sum_trans += 1;
        if(rr < 5) hit_at_5_sum_trans += 1;
        if(rr < 3) hit_at_3_sum_trans += 1;
        if(rr < 1) hit_at_1_sum_trans += 1;

        if(tmptest - 1 >= trans_score(h,r,t) - 0.00001) {
            int tmp_l = use_trans_l(h,r,t); int tmp_r = use_trans_r(h,r,t);
            sum = sum + tmp_l + tmp_r;
            mrr_sum = mrr_sum + 1.0 / (tmp_l + 1) + 1.0 / (tmp_r + 1);
            if(tmp_l < 10) hit_at_10_sum += 1;
            if(tmp_l < 5) hit_at_5_sum += 1;
            if(tmp_l < 3) hit_at_3_sum += 1;
            if(tmp_l < 1) hit_at_1_sum += 1;

            if(tmp_r < 10) hit_at_10_sum += 1;
            if(tmp_r < 5) hit_at_5_sum += 1;
            if(tmp_r < 3) hit_at_3_sum += 1;
            if(tmp_r < 1) hit_at_1_sum += 1;
            continue;
        }
        cout << tmptest << "," << trans_score(h,r,t) << endl;
        double tmp = 1;
        for(int i = 0;i < entities;i ++){
            if(knowledgebase.count(trip(i,r,t)) || testbase.count(trip(i,r,t))) {continue;}
            double tmps = reasoning(i,r,t);
            if(tmps < tmptest) {sum ++;tmp = tmp + 1;}
        }
        cout << "L:" << tmp << "," <<use_trans_l(h,r,t) + 1 << endl;

        mrr_sum += 1 / tmp;
        if (tmp <= 10) hit_at_10_sum += 1;
        if (tmp <= 5) hit_at_5_sum += 1;
        if (tmp <= 3) hit_at_3_sum += 1;
        if (tmp <= 1) hit_at_1_sum += 1;
        tmp = 1;
        for(int i = 0;i < entities;i ++){
            if(knowledgebase.count(trip(h,r,i)) || testbase.count(trip(h,r,i))) {continue;}
            double tmps = reasoning(h,r,i);
            if(tmps < tmptest) {sum ++;tmp = tmp + 1;}
        }
        cout << "R:" << tmp << "," <<use_trans_r(h,r,t) + 1 << endl;
        mrr_sum += 1 / tmp;
        if (tmp <= 10) hit_at_10_sum += 1;
        if (tmp <= 5) hit_at_5_sum += 1;
        if (tmp <= 3) hit_at_3_sum += 1;
        if (tmp <= 1) hit_at_1_sum += 1;

    }

    FILE* ansfile = fopen("ans.txt","w");
    fprintf(ansfile,"reasoning:sum of rank, sum of reciprocal ranking,sum of hit@1,sum of hit@3,sum of hit@5,sum of hit@10:%d,%f,%d,%d,%d,%d\n",sum,mrr_sum,hit_at_1_sum,hit_at_3_sum,hit_at_5_sum,hit_at_10_sum);
    fprintf(ansfile,"trans:sum of rank, sum of reciprocal ranking,sum of hit@1,sum of hit@3,sum of hit@5,sum of hit@10:%d,%f,%d,%d,%d,%d\n",sum_trans,mrr_sum_trans,hit_at_1_sum_trans,hit_at_3_sum_trans,hit_at_5_sum_trans,hit_at_10_sum_trans);
    fclose(ansfile);
    return;
}

int main(int argc,char**argv){
    prepare();
	trans = argv[1];
	if (argv[2] == "t") test_experiment();
    else formal_experiment();
    return 0;
}
