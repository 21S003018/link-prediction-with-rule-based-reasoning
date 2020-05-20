## intrduction

This is a combined framework integrating embedding method and rule-based reasonging to implement better link prediction.

## operating method

1. Before running the reasoning, you must prepare the data well. There are some simple constraints about the data to be used.

   1. A raw data set should consist of six files, w.r.t entity2id, relation2id, train, test, rule and a valid set is not a must.

   2. The rule file contains the rules that are essential in reasoning. We get these rules throw some open work, here we use [amie](https://www.mpi-inf.mpg.de/departments/databases-and-information-systems/research/yago-naga/amie/). The format of a rule is like:

      *?f  /film/director/film  ?a  ?b  /influence/influence_node/peers./influence/peer_relationship/peers  ?f   => ?a  /film/film/edited_by  ?b	1*

      rule.txt is not the source file of an official data set, we shoule use amie algorithm to generate them with: **java -jar amie_plus.jar test.txt > ans.txt**. Before running amie_plus.jar, the data in test.txt should meet the form of tsv and each line a triplet of (h,r,t), where h and t are both entities and r is a relation.


2. Reasoning will be operate by means of embedding model to get the embedding of entities and relations. The work of thu can be used  https://github.com/thunlp/Fast-TransX . Three examples have been existing in the folders of transE, transH and transR respectively. These examples are about FB15k data set.

3. After the first two steps, some other pre-parations follow. And these codes are contained in preprocess.py.

   - **pyhon preprocess.py 1** simplify_rules
- **pyhon preprocess.py 2** construct_knowledge_base
   - **pyhon preprocess.py 3** rating rules
- **pyhon preprocess.py 4** simplify test set
   - **pyhon preprocess.py 5** filter test set

4. compile reasoning with **g++ -std=c++11 -O2 reasoning.cpp**, and run with **./a.out X**

   1. **./a.out E** denotes the embedding model is transE
   2. **./a.out H**
   3. **./a.out R**
   
   note that it will cost a long time with nearly three days, so you can use **./a.out X t** for a small test.

## experiment result

### environment

LSB Version:	core-4.1-amd64:core-4.1-noarch
Distributor ID:	CentOS
Description:	CentOS Linux release 7.7.1908 (Core)
Release:	7.7.1908
Codename:	Core
cpu cores	: 16

total    used    free   shared buff/cache  available
Mem:      125G     10G    106G     85M    9.0G    114G

### result

| Method | MR | MRR | Hits@10 | Hits@5 | Hits@3 | Hits@1 |
| :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| TransE |   70.30   |   45.77   |   74.27   |   64.44   |   55.79        |   29.98  |
| EM-RBR(E) |  68.36(**up1.94**)  |   50.01(**+4.24**)   | 76.23(**+1.96**) |  67.84(**+3.40**)  |   60.62(**+4.83**)   |   34.44(**+4.46**)   |
| TransH |      |      |      |      |      |      |
| EM-RBR(H) |      |      |      |      |      |      |
| TransR |      |      |      |      |      |      |
| EM-RBR(R) |      |      |      |      |      |      |