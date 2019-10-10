#############################################################
# COMP9318 Project                                          #
#                                                           # 
#                                                           #
#############################################################

import helper
from sklearn.preprocessing import LabelEncoder
from sklearn.feature_extraction import DictVectorizer
from sklearn import svm
# from sklearn.metrics import accuracy_score

def get_freq_of_tokens_from_list(list_each_sample):
    tokens = {}
    for token in list_each_sample:
        if token not in tokens:
            tokens[token] = 1
        else:
            tokens[token] += 1

    sort_dic = sorted(tokens.items(), key=lambda x: x[1])
    sort_dic.reverse()
    dictionary_sort_dic = {}
    for j in range(len(sort_dic)):
        dictionary_sort_dic[sort_dic[j][0]] = sort_dic[j][1]
    return dictionary_sort_dic


def get_freq_of_tokens_from_file(file_each_sample):
    tokens = {}
    for token in file_each_sample.split(' '):
        if token not in tokens:
            tokens[token] = 1
        else:
            tokens[token] += 1

    sort_dic = sorted(tokens.items(), key=lambda x: x[1])
    sort_dic.reverse()
    dictionary_sort_dic = {}
    for j in range(len(sort_dic)):
        dictionary_sort_dic[sort_dic[j][0]] = sort_dic[j][1]
    return dictionary_sort_dic


# This function is using to sort the dictionary according to the value
def sort_dictionary(class0_has_only):
    sort_dic_class0_has_only = sorted(class0_has_only.items(), key=lambda x: x[1])
    sort_dic_class0_has_only.reverse()
    dictionary_dic_class0_has_only = {}  # use to store the sorted dictionary
    for j in range(len(sort_dic_class0_has_only)):
        dictionary_dic_class0_has_only[sort_dic_class0_has_only[j][0]] = sort_dic_class0_has_only[j][1]
    return dictionary_dic_class0_has_only

    sorted_whole_sample_weight_dict_list = []  # which is used to store sorted store_whole_sample_weight_dict_list
    for each_sample in store_whole_sample_weight_dict_list:
        sorted_whole_sample_weight_dict_list.append(sort_dictionary(each_sample))
    return sorted_whole_sample_weight_dict_list

def remove_all_instances_of_word_in_list(word_to_remove,sample):
    new_sample = []
    for word in sample:
        if word != word_to_remove:
            new_sample.append(word)
    return new_sample


def remove_and_add_test_article_words(test_sample, delete_list, add_list):
    all_test_sample = test_sample.split('\n')
    L = [line.strip().split(' ') for line in all_test_sample]
    count = 0
    all_garaphs = ''
    for each_paragraph in range(len(all_test_sample)):
        if each_paragraph >= len(delete_list):
            return all_garaphs

        each_paragraph_delete_list = delete_list[each_paragraph]
        each_paragraph_add_list = add_list[each_paragraph]
        for each_word in each_paragraph_delete_list:
            L[each_paragraph] = remove_all_instances_of_word_in_list(each_word, L[each_paragraph])

        for each_word in each_paragraph_add_list:
            L[each_paragraph].append(each_word)
            L[each_paragraph].append(each_word)

        count_word = 0
        garaph = ''
        for each_word in L[each_paragraph]:
            if count_word == 0:
                garaph += each_word
                count_word += 1
            else:
                garaph += ' ' + each_word
        count += 1
        all_garaphs += garaph + '\n'
    return all_garaphs


def words_only_from_sample(each_sample):
    L = []
    for each_pair in each_sample:
        L.append(each_pair[0])
    return L


def dict_to_list(sorted_dict):
    list_sorted_dict = []
    for each_word in sorted_dict:
        list_sorted_dict.append([each_word, sorted_dict[each_word]])
    return list_sorted_dict


# ---------------------------------------------------------------------------------------#
# This function is using to get each sample's relationship about the word and it's appear frequency([{}{,}])
def test_samples_word_to_frequency(x_test, vectorizer):
    arr_test_data = x_test.toarray()
    store_each_sample_dict_list = []
    count = 0
    for i in range(len(arr_test_data)):
        sotore_test_each_word = {}
        for j in range(len(arr_test_data[i])):

            if (arr_test_data[i][j] > 0):
                sotore_test_each_word[vectorizer.feature_names_[j]] = arr_test_data[i][j]
                count += 1
        store_each_sample_dict_list.append(sotore_test_each_word)
    return store_each_sample_dict_list


# This funtion is using to according to the svm training result to find each feature's weight,
# and get the relationship about the word and it's weight
def training_data_coeffecient(training, vectorizer):
    arr = training.coef_[0].toarray()
    sotore_each_word_coeffecient = {}
    count = 0
    for i in range(len(arr[0])):
        count += 1
        sotore_each_word_coeffecient[vectorizer.feature_names_[i]] = arr[0][i]
    sorted_dict = sort_dictionary(sotore_each_word_coeffecient)
    return sorted_dict


# This function is using to find each sample's relationship with word and weight
def dict_between_test_words_and_weight(store_each_sample_dict_list, sorted_dict, vectorizer):
    store_whole_sample_weight_dict_list = []
    store_list = []
    for each_sample_of_test in store_each_sample_dict_list:
        sum_each_sample_weight = 0
        each_sample_weight_dict = {}
        for each_word_of_each_sample in each_sample_of_test:
            # each_word_frequency_of_each_sample = each_sample_of_test[each_word_of_each_sample]
            if (each_word_of_each_sample in sorted_dict):
                each_sample_weight_dict[each_word_of_each_sample] = sorted_dict[each_word_of_each_sample]
                sum_each_sample_weight += sorted_dict[each_word_of_each_sample]  # *each_word_frequency_of_each_sample

        store_list.append(sum_each_sample_weight)
        store_whole_sample_weight_dict_list.append(each_sample_weight_dict)

    sorted_whole_sample_weight_dict_list = []  # which is used to store sorted store_whole_sample_weight_dict_list
    for each_sample in store_whole_sample_weight_dict_list:
        sorted_whole_sample_weight_dict_list.append(sort_dictionary(each_sample))
    return sorted_whole_sample_weight_dict_list

def final_add_or_delete(list_sorted_whole_test_sample_weight_dict_list, list_sorted_dict, test_d):
    list_to_store_all_delete_list = []
    list_to_store_all_add_list = []
    for each_sample in list_sorted_whole_test_sample_weight_dict_list:
        counter_for_training = -1
        delete_list = []
        add_list = []
        for each_word in each_sample:
            if len(delete_list) + len(add_list) == 20:
                break
            if each_word[1] > 0:
                delete_list.append(each_word[0])
            else:
                if len(delete_list) + len(add_list) == 20:
                    break
                else:
                    a = True
                    while (a == True):
                        if list_sorted_dict[counter_for_training] not in each_sample and \
                                        list_sorted_dict[counter_for_training][0] not in add_list:
                            add_list.append(list_sorted_dict[counter_for_training][0])
                            a = False
                        counter_for_training = counter_for_training - 1

        list_to_store_all_delete_list.append(delete_list)
        list_to_store_all_add_list.append(add_list)

    final_result = remove_and_add_test_article_words(test_d, list_to_store_all_delete_list, list_to_store_all_add_list)
    return final_result


def fool_classifier(test_data): ## Please do not change the function defination...
    ## Read the test data file, i.e., 'test_data.txt' from Present Working Directory...
    
    ## You are supposed to use pre-defined class: 'strategy()' in the file `helper.py` for model training (if any),
    #  and modifications limit checking
    strategy_instance=helper.strategy()
    parameters = {'gamma': 'auto', 'C': 0.02, 'kernel': 'linear', 'degree': 3, 'coef0': 0.0}

    # This step is clean the modified_data.txt if there have some context in this file
    modified_data = './modified_data.txt'
    f = open(modified_data, "w")
    f.close()
    ##..................................#
    # first step open strategy().class0 and strategy().class1
    class0_data = strategy_instance.class0  # type is list, 2 dimensions
    class1_data = strategy_instance.class1
    # second step open test_data file

    with open(test_data, 'r') as test_d:
        test = test_d.read()
    # transform class0_data(list) to become dictionary and label them
    features_and_labels = []
    store_test_features = []
    for line in class0_data:
        #         if(line != ''):
        class0_dict = get_freq_of_tokens_from_list(line)
        features_and_labels.append((class0_dict, 0))
    for line in class1_data:
        #         if(line != ''):
        class1_dict = get_freq_of_tokens_from_list(line)
        features_and_labels.append((class1_dict, 1))
        #     print('len(features_and_labels)', len(features_and_labels))

    # transform test(txt file) to becaome dictionary and labe them
    for line in test.split('\n'):
        if (line != ''):
            test_dict = get_freq_of_tokens_from_file(line)
            store_test_features.append((test_dict, 1))
            # transform features_and_labels to become x_train(matrix) and y_train(array)
    encoder = LabelEncoder()
    vectorizer = DictVectorizer(dtype=int, sparse=True)

    x_train, y_train = list(zip(*features_and_labels))
    x_train = vectorizer.fit_transform(x_train)
    y_train = encoder.fit_transform(y_train)

    x_test, y_test = list(zip(*store_test_features))
    x_test = vectorizer.transform(x_test)
    #     print(x_train.shape)

    # According to the svm(Support Vector Machine) to do the training to train training data(540, 5178)
    training = strategy_instance.train_svm(parameters, x_train, y_train)
    #     print(training)
    # result = training.predict(x_test)  # this is the test data's predict result
    # this function is using to get the relationship about the word and it's appear frequency
    store_each_test_sample_frequency_dict_list = test_samples_word_to_frequency(x_test, vectorizer)

    # This funtion is using to according to the svm training result to find each feature's weight,
    # and get the relationship about the word and it's weight
    sorted_traing_data_word_weight_dict = training_data_coeffecient(training, vectorizer)

    # This fuction is using to get each test sample's relationship with word and weight
    sorted_whole_test_sample_word_weight_dict_list = dict_between_test_words_and_weight(
        store_each_test_sample_frequency_dict_list, sorted_traing_data_word_weight_dict, vectorizer)

    ## Your implementation goes here....#
    # This is using to store the traing_data_word_weight(2 demensions list) which is used to compare the test sample's word weight
    sorted_traing_data_word_weight_list = dict_to_list(sorted_traing_data_word_weight_dict)

    # This is using to store the test_data_word_weight(3demesins list) which is used to compare the whole training sample's word weight
    sorted_whole_test_sample_word_weight_3_demesions_list = []
    for each_sample in sorted_whole_test_sample_word_weight_dict_list:
        sorted_whole_test_sample_word_weight_3_demesions_list.append(dict_to_list(each_sample))
        #     print('sorted_whole_test_sample_word_weight_3_demesions_list----------', sorted_whole_test_sample_word_weight_3_demesions_list)
    #
    final_rsult = final_add_or_delete(sorted_whole_test_sample_word_weight_3_demesions_list,
                                      sorted_traing_data_word_weight_list, test)


    ## Write out the modified file, i.e., 'modified_data.txt' in Present Working Directory...
    with open(modified_data, 'w') as modified:
        modified.write(final_rsult)
    
    
    ## You can check that the modified text is within the modification limits.
    modified_data='./modified_data.txt'
    assert strategy_instance.check_data(test_data, modified_data)
    return strategy_instance ## NOTE: You are required to return the instance of this class.


test_data='./test_data.txt'
strategy_instance = fool_classifier(test_data)
