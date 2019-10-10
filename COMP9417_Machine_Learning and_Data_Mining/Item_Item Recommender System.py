import numpy as np
from numpy import *
import pandas as pd
from math import sqrt


def read_file(file):
    prefer = []
    for line in open(file, 'r'):
        (userid, movieid, rating) = line.split()
        if((userid, movieid, rating) == ''):
            continue
        uid = int(userid.split('.')[0])
        mid = int(movieid.split('.')[0])
        rat = float(rating.split('.')[0])
        prefer.append([uid, mid, rat])
    data = array(prefer)
    return data

def User_split_data(file_path, size):
    # read data
    prefer = []
    user_lst, item_lst = [], []
    train_data, test_data = [], []
    for line in open(file_path, 'r'):
        (userid, movieid, rating, ts) = line.split()
        uid = int(userid)
        mid = int(movieid)
        rat = float(rating)
        user_lst.append(uid)
        item_lst.append(mid)
        prefer.append([uid, mid, rat])
    data = array(prefer)

    shuffled_userlst = np.unique(user_lst)
    np.random.seed(5)
    np.random.shuffle(shuffled_userlst)
    cal_user_num = []
    cal_movies_num = []
    train_user_lst = shuffled_userlst[:int(len(shuffled_userlst) * 0.8)]
    test_user_lst = shuffled_userlst[int(len(shuffled_userlst) * 0.8):]
    for line in data:
        if line[0] in train_user_lst:
            train_data.append(line)
            cal_user_num.append(line[0])
            cal_movies_num.append(line[1])
        elif line[0] in test_user_lst:
            rand = random.random()
            if rand < size:
                train_data.append(line)
                cal_user_num.append(line[0])
                cal_movies_num.append(line[1])
            else:
                test_data.append(line)
                cal_user_num.append(line[0])
                cal_movies_num.append(line[1])
    train_data = array(train_data)
    test_data = array(test_data)
    number_of_users = int(max(cal_user_num))
    number_of_moives =  int(max(cal_movies_num))
    return number_of_users, number_of_moives, train_data, test_data, test_user_lst


def user_item_array_function(number_of_users, number_of_moives, whole_data):
    user_item_array = np.zeros((number_of_users, number_of_moives))
    for i in range(len(whole_data)):
        user_item_array[int(whole_data[int(i)][0]) - 1][int(whole_data[int(i)][1]) - 1] = whole_data[int(i)][2]
    # the row operation to get i-th user's average rating
    users_average = []
    for i in range(number_of_users):
        sum_ith_use_rating = sum(user_item_array[i])
        count_number_of_ith_user_item = sum([1 for i in user_item_array[i] if i != 0.0 ])
        if(count_number_of_ith_user_item != 0):
            ith_user_average = sum_ith_use_rating / count_number_of_ith_user_item
        else:
            ith_user_average = 0
        users_average.append(ith_user_average)

    # the column operation to get i-th item's average rating
    items_average = []
    for i in range(number_of_moives):
        sum_ith_item_rating = sum(user_item_array[:, i])
        count_number_of_ith_item_item = sum([1 for i in user_item_array[:, i] if i != 0.0 ])
        if(count_number_of_ith_item_item == 0):
            items_average.append(0)
        else:
            ith_item_average = sum_ith_item_rating / count_number_of_ith_item_item
            items_average.append(ith_item_average)
    return users_average, items_average, user_item_array

def pearson_colleration_similarity(number_of_users, number_of_moives, items_average,user_item_array):
    pearson_colleration_sim_array = np.zeros((number_of_moives, number_of_moives))
    for ith_movie in range(number_of_moives):
        ith_column_movie = user_item_array[:, ith_movie]
        ith_movie_average_rating = items_average[ith_movie]
        ith_movie_average_rating_array = array([float(i) if i == 0 else 1 for i in ith_column_movie])*ith_movie_average_rating
        Rui_minus_Riaverage_vector = ith_column_movie - ith_movie_average_rating_array
        ith_square_sum = np.dot(Rui_minus_Riaverage_vector, Rui_minus_Riaverage_vector)

        for jth_movie in range(ith_movie+1, number_of_moives):
            jth_column_movie = user_item_array[:, jth_movie]
            jth_movie_average_rating = items_average[jth_movie]
            jth_movie_average_rating_array = array([float(i) if i == 0 else 1 for i in jth_column_movie])* jth_movie_average_rating
            Ruj_minus_Rjaverage_vector = jth_column_movie - jth_movie_average_rating_array
            ith_jth_times_sum = np.dot(Rui_minus_Riaverage_vector, Ruj_minus_Rjaverage_vector)
            jth_square_sum = np.dot(Ruj_minus_Rjaverage_vector, Ruj_minus_Rjaverage_vector)
            if(sqrt(ith_square_sum)*sqrt(jth_square_sum) != 0):
                Sim_ith_jth = ith_jth_times_sum / (sqrt(ith_square_sum) * sqrt(jth_square_sum))
                pearson_colleration_sim_array[ith_movie][jth_movie] = Sim_ith_jth
                pearson_colleration_sim_array[jth_movie][[ith_movie]] = Sim_ith_jth
    for i in range(number_of_moives):
        pearson_colleration_sim_array[i][i] = 1
    return pearson_colleration_sim_array

def adjusted_consine_similarity(number_of_users, number_of_moives, users_average,user_item_array):
    adjusted_consine_sim_array = np.zeros((number_of_moives, number_of_moives))
    user_average_rating_array = array(users_average)
    for ith_movie in range(number_of_moives):
        ith_column_movie = user_item_array[:, ith_movie]
        ith_Ru_average_rating_array = array([float(i) if i == 0 else 1 for i in ith_column_movie]) * user_average_rating_array
        Rui_minus_Ruaverage_vector = ith_column_movie - ith_Ru_average_rating_array
        ith_square_sum = np.dot(Rui_minus_Ruaverage_vector, Rui_minus_Ruaverage_vector)
        sqrt_ith_square_sum = sqrt(ith_square_sum)

        for jth_movie in range(ith_movie + 1, number_of_moives):
            jth_column_movie = user_item_array[:, jth_movie]
            jth_Ru_average_rating_array = array(
                [float(i) if i == 0 else 1 for i in jth_column_movie]) * user_average_rating_array
            Ruj_minus_Ruaverage_vector = jth_column_movie - jth_Ru_average_rating_array
            ith_jth_times_sum = np.dot(Rui_minus_Ruaverage_vector, Ruj_minus_Ruaverage_vector)
            jth_square_sum = np.dot(Ruj_minus_Ruaverage_vector, Ruj_minus_Ruaverage_vector)
            sqrt_jth_square_sum = sqrt(jth_square_sum)
            if (sqrt_ith_square_sum * sqrt_jth_square_sum != 0):
                Sim_ith_jth = ith_jth_times_sum / (sqrt_ith_square_sum * sqrt_jth_square_sum)
                adjusted_consine_sim_array[ith_movie][jth_movie] = Sim_ith_jth
                adjusted_consine_sim_array[jth_movie][[ith_movie]] = Sim_ith_jth

    for i in range(number_of_moives):
        adjusted_consine_sim_array[i][i] = 1
    return adjusted_consine_sim_array


#---------------------------------predict part-----------------------------------------------------#
def load_simlarity_data(file_path):
    prefer=[]
    for line in open(file_path,'r'):
        a =line.split()
        prefer.append(a)
    data = array(prefer, dtype='float64')
    return data

def each_user_top_prediction(movie_movie_array, user_item_array, number_of_moives, userid):
    ith_user_movies_array = user_item_array[userid]
    rated_movies_dict = {}
    unrated_movie_list = []  # use to store unrated movies' id
    # print('len(ith_user_movies_array)', len(ith_user_movies_array))
    for i in range(len(ith_user_movies_array)):
        if (ith_user_movies_array[i] != 0):
            rated_movies_dict[i] = ith_user_movies_array[i]  # This dict's key is movie id, value is movie's rating
            unrated_movie_list.append(0)
        else:
            unrated_movie_list.append(1)

    unrated_movie_array = array(unrated_movie_list)
    sum_sij_times_Ruj = np.zeros(number_of_moives)
    sum_sij = np.zeros(number_of_moives)
    for each_rated_movie in rated_movies_dict.keys():  # Here, "each_rated_movie" means j , which is belone N(Whole rated movies)
        jth_rated_movie_similarity_array = movie_movie_array[each_rated_movie]

        i_j_unrated_similarity_array = unrated_movie_array * jth_rated_movie_similarity_array  # "i_j_unrated_similarity_array"means(S_ij)
        sij_times_Ruj = i_j_unrated_similarity_array * rated_movies_dict[each_rated_movie]  # now we get "sij*Ruj"
        sum_sij_times_Ruj += sij_times_Ruj
        sum_sij += abs(i_j_unrated_similarity_array)

    # whole_predictions = sum_sij_times_Ruj / sum_sij

    whole_predictions_dict = {}
    for i in range(number_of_moives):
        if(sum_sij[i] != 0.0):
            whole_predictions_dict[i] = sum_sij_times_Ruj[i] / sum_sij[i]

    sort_whole_prdictions_dict_list = sorted(whole_predictions_dict.items(), key=lambda x: x[1])
    sort_whole_prdictions_dict_list.reverse()
    # select the top 100 predicton
    sorted_prdictions_top_100 = sort_whole_prdictions_dict_list[:100]
    top_100_movies_id = []
    top_100_moviesid_rating = []
    for top_predicted_moive_id in sorted_prdictions_top_100:
        top_100_moviesid_rating.append((top_predicted_moive_id[0] + 1, top_predicted_moive_id[1]))#tuple(id, rating)
        top_100_movies_id.append(top_predicted_moive_id[0] + 1)
    return top_100_movies_id, top_100_moviesid_rating


#----------------------------------------RMSE part----------------------------------------------

def read_test_file(test_file):
    preprocess = []
    for line in open(test_file, 'r'):
        (userid, movieid, rating) = line.split()
        uid = int(userid)
        mid = int(movieid)
        ra = float(rating)
        preprocess.append([uid, mid, ra])
    data = array(preprocess)
    return data


def read_predicted_file(p_file):
    propre_predicted_data_dict = {}
    for line in open(p_file, 'r'):
        (userid, context) = line.split(':')
        uid = int(userid.strip('{'))
        each_user_prediction = context.strip('}\n').strip(' []').split('),')
        each = []
        for i in each_user_prediction:
            each.append(i.strip(' ()'))
        id_ra_dict = {}
        for j in each:
            (user_id, rating) = j.split(', ')
            id_ra_dict[int(user_id)] = float(rating)
        propre_predicted_data_dict[uid] = id_ra_dict

    return propre_predicted_data_dict



#----------------------final part--------------------------------------------
def similarity_mian_function(file_address):
    number_of_users, number_of_moives, train_data, test_data, test_user_lst = User_split_data(file_address, size=0.2)
    # number_of_users, number_of_moives, whole_data = read_file(file_address)
    users_average, items_average, user_item_array = user_item_array_function(number_of_users, number_of_moives, train_data)
    test_file = open('test_file.csv', 'w')
    test_file.close()
    test_file = open('test_file.csv', 'a')
    for i in test_data:
        test_file.write(str(int(i[0]))+'\t'+str(int(i[1]))+'\t'+str(int(i[2])))
        test_file.write('\n')
    test_file.close()


    train_file = open('train_file.csv', 'w')
    train_file.close()
    train_file = open('train_file.csv', 'a')
    for j in train_data:
        train_file.write(str(int(j[0]))+'\t'+str(int(j[1]))+'\t'+str(int(j[2])))
        train_file.write('\n')
    train_file.close()

    pearson_sim = pearson_colleration_similarity(number_of_users, number_of_moives, items_average,user_item_array)
    with open('Pearson Similarity.csv', 'w') as ps:
        list_pearson_sim = pearson_sim.tolist()
        for j in range(len(list_pearson_sim)):
            for i in list_pearson_sim[j]:
                ps.write(str(i)+'\t')
            ps.write('\n')

    adjusted_consine_sim_array =  adjusted_consine_similarity(number_of_users, number_of_moives, users_average,user_item_array)
    with open('Adjusted Cosine Similarity.csv', 'w') as ps:
        list_pearson_sim = adjusted_consine_sim_array.tolist()
        for j in range(len(list_pearson_sim)):
            for i in list_pearson_sim[j]:
                ps.write(str(i)+'\t')
            ps.write('\n')


    return number_of_users, number_of_moives, test_user_lst

def prediction_main_function(number_of_users, number_of_moives, test_file, similarity_file):
    whole_data = read_file(test_file)
    users_average, items_average, user_item_array = user_item_array_function(number_of_users, number_of_moives, whole_data)
    movie_movie_array = load_simlarity_data(similarity_file)
    final_file = open('Predicton.csv', 'w')
    final_file.close()
    final_file_with_rating = open('Predicton_with_rating.csv', 'w')
    final_file_with_rating.close()
    final_file_with_rating = open('Predicton_with_rating.csv', 'a')
    final_file = open('Predicton.csv', 'a')
    for user_id in range(number_of_users):
        each_user_prediction_dict = {}
        each_user_movieid_rating_prediction_dic = {}
        top_100_movies_id, top_100_moviesid_rating = each_user_top_prediction(movie_movie_array, user_item_array, number_of_moives, user_id)
        each_user_prediction_dict[user_id+1] = top_100_movies_id
        each_user_movieid_rating_prediction_dic[user_id+1] = top_100_moviesid_rating
        final_file.write(str(each_user_prediction_dict)+'\n')
        final_file_with_rating.write(str(each_user_movieid_rating_prediction_dic)+'\n')
    final_file_with_rating.close()
    final_file.close()


def RMSE(prediction_file, test_file):
    test_data = read_test_file(test_file)
    propre_predicted_data = read_predicted_file(prediction_file)
    key = propre_predicted_data.keys()

    whole_test_dict = {}
    for i in test_data:
        whole_test_dict[int(i[0])] = []
    for i in test_data:
        userid = int(i[0])
        if (userid in key):
            moiveid = int(i[1])
            if (moiveid in propre_predicted_data[userid].keys()):
                predicted_rating = propre_predicted_data[userid][moiveid]
                real_rating = float(i[2])
                square_error = (real_rating - predicted_rating) ** 2
                whole_test_dict[userid].append(square_error)


    whole_root_mean_square_error = {}
    for each_key in whole_test_dict.keys():
        count_num = sum([1 for i in whole_test_dict[each_key]])
        each_key_error_sum = sum(whole_test_dict[each_key])
        each_key_root_mean_square_error = sqrt(each_key_error_sum / (count_num + 1))
        whole_root_mean_square_error[each_key] = each_key_root_mean_square_error

    sum_whole_RMSE = 0
    sum_users = 0
    for i in whole_root_mean_square_error.keys():
        sum_users += 1
        sum_whole_RMSE += whole_root_mean_square_error[i]
    average_RMSE = sum_whole_RMSE / sum_users
    return average_RMSE




number_of_users = 943
number_of_moives = 1682

raw_file = './ml-100k/raw.data'
number_of_users, number_of_moives, test_user_lst = similarity_mian_function(raw_file)#this part is used to do the similarity computation
file = 'Pearson Similarity.csv'
train_file = './train_file.csv'
test_file = './test_file.csv'
prediction_main_function(number_of_users, number_of_moives, train_file, file)#this part is used to do the prediction
predic_file = './Predicton_with_rating.csv'
average_RMSE = RMSE(predic_file, test_file)##this part is used to compute Root Mean Square Error
