//
// Created by alaeddine on 10/26/17.
//

#ifndef FLOWOVER_MATRIX_H
#define FLOWOVER_MATRIX_H


#include "../includes.h"
#include "../queues_over/sync_queue.h"
#include "../constants_and_definitions.h"
#include "../helper_class.h"
#include "../server_things.h"

class server_things;

class computer_client;

template<typename T>
using my_distribution = std::conditional_t<std::is_integral<T>::value,
        std::uniform_int_distribution<T>,
        std::conditional_t<std::is_floating_point<T>::value,
                std::uniform_real_distribution<T>,
                void>
>;

template<class T>
class mat {
public:
    mat<T>() {
        cols = 0;
        rows = 0;
        size = 0;
        data = std::vector<T>(0,0);

    };

    mat<T>(unsigned int rows, unsigned int cols) {
        this->cols = cols;
        this->rows = rows;
        this->size = rows * cols;
        data = std::vector<T>(size,0);

    };

    const unsigned int &getRows() const {
        return rows;

    };

    const unsigned int &getCols() const {
        return cols;

    };

    void print() {
        std::stringstream ss;
        for (unsigned int i = 0; i < rows; ++i) {
            for (unsigned int j = 0; j < cols; ++j) {
                ss << std::to_string(at(i, j)) + "\t";
            }
            ss << "\n";
        }
        std::cout << ss.str();
    };

    void print_to_file(string name) {
        std::stringstream ss;
        for (unsigned int i = 0; i < rows; ++i) {
            for (unsigned int j = 0; j < cols; ++j) {
                ss << std::to_string(at(i, j)) + "\t";
            }
            ss << "\n";
        }
        std::ofstream myfile;
        myfile.open(name + ".mat");
        myfile << ss.str();
        myfile.close();
    };

    void randomize(T min, T max) {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::default_random_engine e1(rd());//Standard mersenne_twister_engine seeded with rd()
        my_distribution<T> dis(min, max);
        for (int i = 0; i < size; ++i) {
            data[i] = dis(e1);
        }
    }

    unsigned int getSize() const {
        return this->size;
    };

    T &at(unsigned int i, unsigned int j) {
        return data[i * cols + j];

    };

    mat<T> get_transpose() {
        mat<T> mat_transpose(cols, rows);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                mat_transpose.at(j, i) = at(i, j);
            }
        }
        return mat_transpose;

    };


    mat<T> multiply(mat mat1) {
        mat matres = mat<T>();
        if (cols != mat1.getRows()) {
            return matres;
        }
        matres = mat<T>(rows, mat1.getCols());

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < mat1.getCols(); ++j) {
                int k = 0;
                T sum = 0;
                while (k < mat1.getRows()) {

                    sum += at(i, k) * mat1.at(k, j);

                    k++;
                }
                matres.at(i, j) = sum;
            }
        }
        return matres;
    };


    mat<T> distributed_multiply(byte tag, mat matrix2, atomic_bool *available_slots, computing_client **clients,
                                sync_queue<vector<byte >> &multiplication_message_queue) {
        atomic_int count = 0;
        mat<T> matrix_res;

        if (cols != matrix2.getRows()) {
            helper_class::display_error_syncronized_ln("Invalid Matrix multiplication");
            return matrix_res;
        }

        matrix_res = mat<T>(rows, matrix2.getCols());
       thread pdfp(&mat<T>::prepare_data, this, ref(tag), ref(matrix2), ref(available_slots), ref(clients),
                    ref(multiplication_message_queue));


        while (count < matrix_res.getSize()) {

            vector<byte> buf;
            multiplication_message_queue.pop(buf);

            if (buf[TAG_LOCATION] == tag) {
                ++count;
                auto *i = new short;
                memcpy(i, buf.data() + ROW_INDEX_LOCATION, sizeof(short));
                auto *j = new short;
                memcpy(j, buf.data() + COLUMN_INDEX_LOCATION, sizeof(short));
                matrix_res.at(*i, *j) = helper_class::bytearray_to_element<T>(buf.data(),
                                                                              MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION);
                delete i ,j;
                helper_class::display_info_syncronized_ln("count :" +to_string(count));

            } else {
                multiplication_message_queue.push(buf);
            }


        }

        pdfp.join();
        return matrix_res;
    }

    mat inverse() {
        mat matres = mat();
        if (rows != cols) {
            helper_class::display_error_syncronized_ln("Inverse: ");
            return matres;
        }

        // Find determinant of A[][]
        double det = determinant(rows);
        helper_class::display_info_syncronized_ln("det : " + to_string(det));
        if (det == 0) {
            helper_class::display_error_syncronized_ln("Singular matrix, can't find its inverse");
            return matres;
        }
        matres = mat(rows, cols);
        // Find adjoint

        mat adj = adjoint();
        helper_class::display_info_syncronized_ln("adj:");
        adj.print();
        // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < rows; j++)
                matres.at(i, j) = adj.at(i, j) / det;

        return matres;
    }
    mat inverse_with_lu(){
        mat matres = mat();
        if (rows != cols) {
            helper_class::display_error_syncronized_ln("Inverse: ");
            return matres;
        }

        //UL decomposition
        mat U(cols,cols);
        mat L(cols,cols);

        for (int k = 0; k < cols ; ++k) {

            for (int j = k; j < cols ; ++j) {
                double sum =0;
                for (int p = 0; p <k ; ++p) {
                    sum += L.at(k, p) * U.at(p, j);
                }
                U.at(k,j) = at(k,j) -sum;
            }
            if(U.at(k,k)==0)return matres;
            L.at(k,k) = 1;
            for (int i = k+1; i <cols ; ++i) {
                double sum =0;
                for (int p = 0; p <k ; ++p) {
                    sum += L.at(i, p) * U.at(p, k);
                }

                L.at(i,k)=( at(i,k) -sum) /U.at(k,k);
            }
        }

        //solve LU*inv(A)=I to find inverse
        // L * UNKNOWN(D) = I
        // U * UNKNOWN(X) = D
        //Replace I with any B to solve A*X=B in which X <=> matres

        mat I (cols,cols);
        mat D (cols,cols);
        for (int l = 0; l <cols ; ++l) {
            I.at(l,l)=1;
        }
        matres = mat(cols,I.cols);
        for (int k = 0; k < I.cols ; ++k) {
            for (int i = 0; i < cols; ++i) {
                double sum =0;
                for (int j = 0; j < i ; ++j) {
                    sum+= L.at(i,j)*D.at(j,k);
                }
                D.at(i,k) = I.at(i,k) - sum;
            }
            for (int i = cols-1; i >= 0; --i) {
                double sum =0;
                for (int j = i+1; j < cols  ; ++j) {
                    sum+= U.at(i,j)*matres.at(j,k);
                }
                matres.at(i,k) =  (D.at(i,k) - sum)/U.at(i,i);
            }
        }

        return  matres;

    }
    mat inverse_nm_with_lu(){
        mat matres;
        mat trans = get_transpose();
    }
private:

    double determinant(int n) {

        mat U(n,n);
        mat L(n,n);
        double curr;
        double  det =1 ;
        for (int k = 0; k < n ; ++k) {

            for (int j = k; j < n ; ++j) {
                double sum =0;
                for (int p = 0; p <k ; ++p) {
                    sum += L.at(k, p) * U.at(p, j);
                }
                U.at(k,j) = at(k,j) -sum;
            }
            if(U.at(k,k)==0)return 0;
            L.at(k,k) = 1;
            for (int i = k+1; i <n ; ++i) {
                double sum =0;
                for (int p = 0; p <k ; ++p) {
                    sum += L.at(i, p) * U.at(p, k);
                }

                L.at(i,k)=( at(i,k) -sum) /U.at(k,k);
            }
        }
        for (int l = 0; l < n ; ++l) {
            det*= U.at(l,l);
        }
        return det;



    }

    mat adjoint() {

        mat matres = mat(rows, cols);
        if (rows != cols) {
            helper_class::display_error_syncronized_ln("Adjoint: ");
            return matres;
        }
        if (rows == 1) {
            matres.at(0, 0) = 1;
            return matres;
        }

        // temp is used to store cofactors of A[][]
        int sign = 1;
        mat temp;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rows; j++) {
                // Get cofactor of A[i][j]
                temp = getCofactor(i, j,rows);

                // sign of adj[j][i] positive if sum of row
                // and column indexes is even.
                sign = ((i + j) % 2 == 0) ? 1 : -1;

                // Interchanging rows and columns to get the
                // transpose of the cofactor matrix
                matres.at(j, i) = (sign) * (temp.determinant(rows - 1));
            }
        }
    }

    mat getCofactor(int p, int q , int n) {
        mat matres = mat(rows, cols);
        if (rows != cols) {
            helper_class::display_error_syncronized_ln("Cofactor: ");
            return matres;
        }
        int i = 0, j = 0;

        // Looping for each element of the matrix
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                //  Copying into temporary matrix only those element
                //  which are not in given row and column
                if (row != p && col != q) {
                    matres.at(i, j++) = at(row, col);

                    // Row is filled, so increase row index and
                    // reset col index
                    if (j == n - 1) {
                        j = 0;
                        i++;
                    }
                }
            }
        }
    }

    void prepare_data(byte tag, mat matrix2, atomic_bool *available_slots, computing_client **clients,
                      sync_queue<vector<byte >> &multiplication_message_queue) {
        size_t size_of_data = sizeof(T);

        int rows1 = rows;
        int rows2 = matrix2.getRows();
        int cols2 = matrix2.getCols();
        //Messages to operate
        for (unsigned short i = 0; i < rows1; ++i) {
            for (unsigned short j = 0; j < cols2; ++j) {
                unsigned short  size= 2*rows2 * size_of_data+LENGTH_PADDING;
                vector<byte> vector_(size);

                //buffer init
                memcpy(vector_.data() + BUFFER_LENGTH_LOCATION, &size, sizeof(unsigned short));
                vector_[ID_LOCATION] = APP_ID;
                vector_[INS_LOCATION] = INS_MULTIPLY_AND_SUM_R_X_C;
                vector_[TAG_LOCATION] = tag;
                memcpy(vector_.data() + ROW_INDEX_LOCATION, &i, sizeof(unsigned short));
                memcpy(vector_.data() + COLUMN_INDEX_LOCATION, &j, sizeof(short));
                memcpy(vector_.data() + MATRIX_MULTIPLICATION_LENGTH_LOCATION, &rows2, sizeof(unsigned short));

                int row_begin = MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION;
                int column_begin = rows2 * size_of_data + row_begin;
                int k = 0;
                while (k < rows2) {

                    union {
                        T number;
                        byte buffer[sizeof(T)];
                    } converter;
                    converter.number = at(i, k);
                    //insert one value from row to buffer
                    memcpy(vector_.data() + row_begin, converter.buffer, size_of_data);
                    //insert one value from col to buffer
                    converter.number = matrix2.at(k, j);
                    memcpy(vector_.data() + column_begin, converter.buffer, size_of_data);

                    row_begin += size_of_data;
                    column_begin += size_of_data;
                    k++;
                }
                // choose connected client to send

                if (!available_slots[j % WAI_FOR_CLIENTS]) {
                    clients[j % WAI_FOR_CLIENTS]->message_dispatcher_queue.push(vector_);
                }

#ifdef HANDICAP
                usleep(HANDICAP_VALUE);
#endif
            }

        }

    };


    std::vector<T> data;

    unsigned rows;
    unsigned cols;
    unsigned size;
};


#endif //FLOWOVER_MATRIX_H
