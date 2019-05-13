import numpy as np
from sklearn.datasets import make_classification
import sys
import os

if __name__ == '__main__':
    path = sys.argv[1]
    if not os.path.exists(path):
        os.makedirs(path)

    s_X, s_y = make_classification(n_samples = int(1e2), n_features = 2, n_informative = 2, n_redundant = 0, n_repeated = 0, n_classes = 2, random_state = 43)

    b_X, b_y = make_classification(n_samples = int(1e7), n_features = 100, n_informative = 50, n_redundant = 20, n_repeated = 30, n_classes = 2, random_state = 43)

    np.savetxt(path + 's_X.csv', s_X, fmt = '%f', delimiter = ',')
    np.savetxt(path + 's_y.csv', s_y, fmt = '%f', delimiter = ',')
    np.savetxt(path + 'b_X.csv', b_X, fmt = '%f', delimiter = ',')
    np.savetxt(path + 'b_y.csv', b_y, fmt = '%f', delimiter = ',')
