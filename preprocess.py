# transform the form of rules in rule.txt
# output them to newrule.txt
trans = "H"
def trans_the_form_of_rule():
    import re
    relation2id = {}
    file = open('rawdata/relation2id.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        relation2id[tmp[0]] = tmp[1]

    def replace_relation_with_id(r):
        def func(matched):
            tmp = matched.group(0)
            return relation2id[tmp]
        return re.sub(r'(/[a-z0-9./_]+)', func, r)

    def formalize(r):
        def func(tmp):
            tmp = tmp.group(0)
            tmp = tmp.replace('  ', ',')
            tmp = tmp.replace('?', '')
            return tmp

        ans = re.sub(r'(\?[a-z0-9 ]+\?[a-z])', func, r)
        ans = ans.replace('  ', '|')
        ans = ans.replace('\t', ';')
        return ans

    file = open('rawdata/rule.txt','r')
    data = file.readlines()
    file.close()
    out = open('newrule.txt','w')
    for tmp in data:
        if tmp == '\t\n':
            continue
        tmp = tmp.replace('   => ','\t')
        tmp = tmp.replace('\n','')
        print(tmp)
        tmp = replace_relation_with_id(tmp)
        tmp = formalize(tmp)
        tmp += '\n'
        print(tmp)
        out.write(tmp)
    out.close()

def trans_the_form_of_kb():
    relation2id = {}
    file = open('rawdata/relation2id.txt', 'r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n', '')
        tmp = tmp.split('\t')
        relation2id[tmp[0]] = tmp[1]

    entity2id = {}
    file = open('rawdata/entity2id.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        entity2id[tmp[0]] = tmp[1]

    kb = []
    file = open('rawdata/train.txt','r')
    kb += file.readlines()
    file.close()
    file = open('rawdata/valid.txt','r')
    kb += file.readlines()
    file.close()

    out = open('knowledgebase.txt','w')
    for tmp in kb:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        ans = entity2id[tmp[0]] + ',' + relation2id[tmp[2]] + ',' + entity2id[tmp[1]] + '\n'
        out.write(ans)
    out.close()

def calculate_score_of_rule(trans='H'):
    import numpy as np
    def get_int_array(x):
        ret = []
        for tmp in x:
            if tmp != '':
                ret.append(float(tmp))
        return np.array(ret)
    relation2vec = {}
    file = open('trans' + trans + '/relation2vec.txt','r')
    data = file.readlines()
    file.close()
    id = 0
    for tmp in data:
        tmp = tmp.replace('\n', '')
        tmp = tmp.split('\t')
        relation2vec[id] = get_int_array(tmp)
        id += 1

    file = open('newrule.txt','r')
    data = file.readlines()
    file.close()
    outdata = []
    for tmp in data:
        sum_vecs = np.array([0.0 for i in range(len(relation2vec[0]))])
        tmp = tmp.split(';')
        lefts = tmp[0].split('|')
        right = tmp[1]
        if len(lefts) == 2:
            P = lefts[0].split(',')
            Q = lefts[1].split(',')
            if P[0] == 'f' or P[0] == 'e':
                sum_vecs -= relation2vec[int(P[1])]
            else:
                sum_vecs += relation2vec[int(P[1])]
            if Q[0] == 'f' or Q[0] == 'e':
                sum_vecs += relation2vec[int(Q[1])]
                xtmp = Q[2]
            else:
                sum_vecs -= relation2vec[int(Q[1])]
                xtmp = Q[0]
        else:
            ttmp = lefts[0].split(',')
            sum_vecs += relation2vec[int(ttmp[1])]
            xtmp = ttmp[2]
        ttmp = right.split(',')
        if xtmp == ttmp[0]:
            sum_vecs += relation2vec[int(ttmp[1])]
        else:
            sum_vecs -= relation2vec[int(ttmp[1])]
        ans = 0.0
        for ttmp in sum_vecs:
            ans += abs(ttmp)
        outdata.append(tmp[0] + ';' + tmp[1] + ';' + str(round(ans / len(sum_vecs),8)) + '\n')
    file = open('trans' + trans + '/newrule.txt','w')
    for tmp in outdata:
        file.write(tmp)
    file.close()

def trans_the_form_of_test():
    relation2id = {}
    file = open('rawdata/relation2id.txt', 'r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n', '')
        tmp = tmp.split('\t')
        relation2id[tmp[0]] = tmp[1]

    entity2id = {}
    file = open('rawdata/entity2id.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        entity2id[tmp[0]] = tmp[1]

    kb = []
    file = open('rawdata/test.txt','r')
    kb += file.readlines()
    file.close()

    out = open('newtest.txt','w')
    for tmp in kb:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        ans = entity2id[tmp[0]] + ',' + relation2id[tmp[2]] + ',' + entity2id[tmp[1]] + '\n'
        out.write(ans)
    out.close()

def filter_test():
    can_be_matched = set()
    file = open('newrule.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        can_be_matched.add(tmp.split(';')[1].split(',')[1])
    file = open('newtest.txt','r')
    data = file.readlines()
    file.close()
    file = open('filtertest.txt','w')
    for tmp in data:
        if can_be_matched.__contains__(tmp.split(',')[1]):
            file.write(tmp)
    file.close()

def trans_the_form_of_train_for_thu():
    relation2id = {}
    file = open('rawdata/relation2id.txt', 'r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n', '')
        tmp = tmp.split('\t')
        relation2id[tmp[0]] = tmp[1]

    entity2id = {}
    file = open('rawdata/entity2id.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        entity2id[tmp[0]] = tmp[1]

    kb = []
    file = open('rawdata/train.txt','r')
    kb += file.readlines()
    file.close()

    out = open('train2id.txt','w')
    for tmp in kb:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        ans = entity2id[tmp[0]] + ' ' + entity2id[tmp[1]] + ' ' + relation2id[tmp[2]] + '\n'
        out.write(ans)
    out.close()

def trans_the_form_of_valid_for_thu():
    relation2id = {}
    file = open('rawdata/relation2id.txt', 'r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n', '')
        tmp = tmp.split('\t')
        relation2id[tmp[0]] = tmp[1]

    entity2id = {}
    file = open('rawdata/entity2id.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        entity2id[tmp[0]] = tmp[1]

    kb = []
    file = open('rawdata/valid.txt','r')
    kb += file.readlines()
    file.close()

    out = open('valid2id.txt','w')
    for tmp in kb:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        ans = entity2id[tmp[0]] + ' ' + entity2id[tmp[1]] + ' ' + relation2id[tmp[2]] + '\n'
        out.write(ans)
    out.close()

def trans_the_form_of_test_for_thu():
    relation2id = {}
    file = open('rawdata/relation2id.txt', 'r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n', '')
        tmp = tmp.split('\t')
        relation2id[tmp[0]] = tmp[1]

    entity2id = {}
    file = open('rawdata/entity2id.txt','r')
    data = file.readlines()
    file.close()
    for tmp in data:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        entity2id[tmp[0]] = tmp[1]

    kb = []
    file = open('rawdata/test.txt','r')
    kb += file.readlines()
    file.close()

    out = open('test2id.txt','w')
    for tmp in kb:
        tmp = tmp.replace('\n','')
        tmp = tmp.split('\t')
        ans = entity2id[tmp[0]] + ' ' + entity2id[tmp[1]] + ' ' + relation2id[tmp[2]] + '\n'
        out.write(ans)
    out.close()

if __name__ == '__main__':
    import sys
    commands = sys.argv[2]
    if commands == '1':
        trans_the_form_of_rule()
    if commands == '2':
        trans_the_form_of_kb()
    if commands == '3':
        calculate_score_of_rule('E')
        calculate_score_of_rule('H')
        calculate_score_of_rule('R')
    if commands == '4':
        trans_the_form_of_test()
    if commands == '5':
        filter_test()
    # filter_test()
    # trans_the_form_of_rule()
    # print('rule form transformed!')
    # trans_the_form_of_kb()
    # print('knowl/edge base form transformed!')
    # calculate_score_of_rule('H')
    # calculate_score_of_rule('E')
    # calculate_score_of_rule('R')
    # print('rule score calculated!')
    # trans_the_form_of_test()
    # print('test form transformed!')
    # trans_the_form_of_valid_for_thu()
    # trans_the_form_of_test_for_thu()
    # import pandas as pd
    # df = pd.read_csv('rule.csv')
    # values = df['PCA Confidence'].values
    # file = open('newrule.txt','r')
    # data = file.readlines()
    # file.close()
    # file = open('newrule.txt','w')
    # index = 0
    # for tmp in data:
    #     tmp = tmp.split(";")
    #     print(tmp[0] + ';' +tmp[1] + ';' + str(1 - values[index]) + '\n')
    #     index += 1
    # file.close()
    # for tmp in data:
    #     tmp = tmp.split(";")
    #     file.write(tmp[0] + ';' +tmp[1] + ';' + str(1 - values[index]) + '\n')
    #     index += 1
    # file.close()
    #
