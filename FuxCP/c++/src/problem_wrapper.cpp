#include "../headers/problem_wrapper.hpp"
#include "../headers/CounterpointProblems/CounterpointProblem.hpp"

#include "../headers/Utilities.hpp"
#include "../headers/CounterpointUtils.hpp"

/**
 * Wraps the Problem constructor.
 * @todo modify this to include any parameters your Problem constructor requires
 * @param size an integer representing the size of the problem
 * @param lower_bound_domain an integer representing the lower bound of the domain of the variables
 * @param upper_bound_domain an integer representing the upper bound of the domain of the variables
 * @return A pointer to a Problem object casted as a void*
 */
void* create_new_problem(int* cantusFirmus, int size, int n_cp, int* splist, int* v_types, int b_mode, int min_skips, int* general_params, 
        int* motion_params, int* melodic, int* specific, int* importance, int t_off, int* scle, int scale_size, 
        int* chromatic, int chrom_size, int* borrow, int borrow_size){

    writeToLogFile("Entered problem_wrapper.cpp . ");

    vector<int> cf(int_pointer_to_vector(cantusFirmus, size));
    vector<int> sp(int_pointer_to_vector(splist, n_cp));
    vector<int> mot(int_pointer_to_vector(motion_params, 3));
    vector<int> vt(int_pointer_to_vector(v_types, n_cp));
    vector<int> sc(int_pointer_to_vector(scle, scale_size));
    vector<int> chr(int_pointer_to_vector(chromatic, chrom_size));
    vector<int> brw(int_pointer_to_vector(borrow, borrow_size));
    vector<int> mel(int_pointer_to_vector(melodic, 8));
    vector<int> gen(int_pointer_to_vector(general_params, 8));
    vector<int> spec(int_pointer_to_vector(specific, 7));
    vector<int> imp(int_pointer_to_vector(importance, 14));
    // return (void*) new Problem(cf, size, n_cp, sp, vt, b_mode, min_skips, gen, mot, mel, spec, imp, t_off, sc, scale_size, chr, chrom_size, brw, borrow_size);
    return create_problem(cf, sp, C, 0, 127, vt, mel, gen, spec, b_mode);
}

/**
 * returns the size of the problem
 * @param sp a void* pointer to a CounterpointProblem object
 * @return an integer representing the size of the problem
 */
int get_size(void* sp){
    return static_cast<CounterpointProblem*>(sp)->getSize();
}

int test_cffi(int n){
    writeToLogFile(to_string(n).c_str());
    return n+1;
}


void delete_pointer(void* p){
    writeToLogFile("deleting counterpointproblem pointer...");
    delete static_cast<CounterpointProblem*>(p);
    writeToLogFile("deleted CounterpointProblem pointer . ");
}

void delete_solver_pointer(void* p){
    delete static_cast<DFS<CounterpointProblem>*>(p);
    writeToLogFile("deleted solver pointer");
}


/**
 * returns the values of the variables for a solution
 * @param sp a void* pointer to a Problem object
 * @return an int* pointer representing the values of the variables
 */
int* return_solution(void* sp){
    return static_cast<CounterpointProblem*>(sp)->return_solution();
}

/**
 * creates a search engine for Problem objects
 * @param sp a void* pointer to a Problem object
 * @return a void* cast of a Base<Problem>* pointer
 */
void* create_solver(void* sp, int type){
    return (void*) make_solver(static_cast<CounterpointProblem*>(sp), type);  // TODO 
}

/**
 * returns the next solution space, it should be bound. If not, it will return NULL.
 * @param solver a void* pointer to a Base<CounterpointProblem>* pointer for the search engine of the problem
 * @return a void* cast of a CounterpointProblem* pointer
 */
void* return_next_solution_space(void* solver){
    writeToLogFile("return_next_solution_space function called");
    return (void*) get_next_solution_space(static_cast<DFS<CounterpointProblem>*>(solver));
}



int search_stopped(void* solver){
    return static_cast<int>(static_cast<Base<CounterpointProblem>*>(solver)->stopped());
}