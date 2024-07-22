#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <condition_variable>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
using namespace std;

const int maxnum = 10;
int counter = 0;
int scores[maxnum] = {0};
int retakescores[maxnum] = {0};
bool quiztaken[maxnum] = {false};
char answer[maxnum] = {};
int activeUser;
int answered;
bool isAnswered = false;
condition_variable cv;
void leaderboard();
mutex cv_m;
string updateQuestion();
void deleteQuestion();
int displayQuestions();
int easyL = 6;
int mediumL = 6;
int hardL = 6;
string easyquestion[100] = {
    "1. The training institution was called __ with the first batch in 1999.\n   [A] Granby Computer Center\n   [B] Granby Computer Learning Center\n   [C] Granby College of Science and Technology\n   [D] Granby Colleges of Science and Technology",

                         "2. What is the name of the School President?\n   [A] Mercurio G. Villanueva, Ph.D, Ed.D, CSP, DBM, MIT\n   [B] Prudencio S. De Ausen MA. Ed.",
                         "3. Somebody from CHED suggested that the school should change a certain word in the name of the school, giving a new name of the institution: __.\n   [A] Granby Computer Center\n   [B] Granby Computer Learning Center\n   [C] Granby College of Science and Technology\n   [D] Granby Colleges of Science and Technology",
                         "4. What is the name of the Corporate President?\n   [A] Mercurio G. Villanueva, Ph.D, Ed.D, CSP, DBM, MIT\n   [B] Prudencio S. De Ausen MA. Ed.",
                         "5. The TESDA suggested that the school should be under a corporation as required by law and so, __ was created.\n   [A] Granby Computer Center\n   [B] Granby Computer Learning Center\n   [C] Granby College of Science and Technology\n   [D] Granby Colleges of Science and Technology",
                         "6. The One-Year Computer Programming course was approved under the Partnership Company name __.\n   [A] Granby Computer Center\n   [B] Granby Computer Learning Center\n   [C] Granby College of Science and Technology\n   [D] Granby Colleges of Science and Technology"};
char easycorrectansweruppercase[100] = {'B', 'A', 'D', 'A', 'C', 'A'};
char easycorrectanswerlowercase[100] = {'b', 'a', 'd', 'a', 'c', 'a'};


string mediumquestion[100] = {"1. Who was the developer of the C programming language?\n   [A] Dennis Ritchie\n   [B] Ken Thompson\n   [C] Bjarne Stroustrup\n   [D] Linus Torvalds",
                           "2. Which of the following is not a data type in C++?\n   [A] int\n   [B] real\n   [C] float\n   [D] bool",
                           "3. Which of the following is a correct comment in C++?\n   [A] /# This is a comment\n   [B] // This is a comment\n   [C] /* This is a comment */\n   [D] # This is a comment",
                           "4. What is the output of the following code snippet?\n   int x = 5;\n   cout << x++ << endl;\n   [A] 6\n   [B] 5\n   [C] Error\n   [D] 4",
                           "5. Which of the following correctly declares a pointer in C++?\n   [A] int *ptr;\n   [B] ptr int;\n   [C] int ptr;\n   [D] ptr *int;",
                           "6. What is the result of 15 % 4 in C++?\n   [A] 3\n   [B] 1\n   [C] 0\n   [D] 4"};
char mediumcorrectansweruppercase[100] = {'A', 'B', 'B', 'B', 'A', 'B'};
char mediumcorrectanswerlowercase[100] = {'a', 'b', 'b', 'b', 'a', 'b'};

string hardquestion[100] = {"1. What is the time complexity of quicksort in the average case?\n   [A] O(n)\n   [B] O(n log n)\n   [C] O(n^2)\n   [D] O(log n)",
                         "2. Which data structure is typically used for implementing priority queues?\n   [A] Array\n   [B] Linked List\n   [C] Binary Heap\n   [D] Stack",
                         "3. What is the space complexity of binary search?\n   [A] O(1)\n   [B] O(log n)\n   [C] O(n)\n   [D] O(n^2)",
                         "4. Which algorithm is used for finding the shortest path in a weighted graph with positive edge weights?\n   [A] Breadth-First Search\n   [B] Depth-First Search\n   [C] Dijkstra's Algorithm\n   [D] Bellman-Ford Algorithm",
                         "5. Which sorting algorithm has the best worst-case time complexity?\n   [A] Quick Sort\n   [B] Merge Sort\n   [C] Bubble Sort\n   [D] Insertion Sort",
                         "6. In computer science, what does the acronym SQL stand for?\n   [A] Structured Query Language\n   [B] Sequential Query Language\n   [C] Standardized Query Language\n   [D] Secondary Query Language"};
char hardcorrectansweruppercase[100] = {'B', 'C', 'A', 'C', 'B', 'A'};
char hardcorrectanswerlowercase[100] = {'b', 'c', 'a', 'c', 'b', 'a'};


string Firstname[maxnum];
string Lastname[maxnum];
string Course[maxnum];
int Yearlevel[maxnum];
string Password[maxnum];

struct User {
    string lastName;
    string firstName;
    string course;
    int yearLevel;
    int score;
    int retakeScore;
    bool quizTaken;
};
string intostring(int num){
    stringstream ss;
    ss << num;
    return ss.str();
}

string getTimestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << ltm->tm_year + 1900 << "-" << ltm->tm_mon + 1 << "-" << ltm->tm_mday << " " << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec;
    return ss.str();
}
void clearArray(char type){
    switch(type){
    case 'e':
            for(int index = 0; index <= easyL ; index++){
                easyquestion[index] = "";
                easycorrectanswerlowercase[index] = ' ';
                easycorrectansweruppercase[index] = ' ';
               
            }
            easyL = 0;
        break;
    case 'm':
        for(int index = 0; index < easyL ; index++){
                mediumquestion[index] = "";
                mediumcorrectanswerlowercase[index] = ' ';
                mediumcorrectansweruppercase[index] = ' ';
               
            }
            mediumL = 0;
        break;
    case 'h':
        for(int index = 0; index < easyL ; index++){
                hardquestion[index] = "";
                hardcorrectanswerlowercase[index] = ' ';
                hardcorrectansweruppercase[index] = ' ';
                
            }
            hardL = 0;
        break;
    default:
        cout << "hahaha";
    }
}
string add_new_element(int&index){
    string question;
    string a;
    string b;
    string c;
    string d;

    cout << "Question :" ;
    cin >> question;
    cout << "[A] :";
    cin >> a;
    cout << "[B] :";
    cin >> b;
    cout << "[C] :";
    cin >> c;
    cout << "[D] :";
    cin >> d;


    return (to_string(index)) + ". "+question+ "\n   " + "[A] " + a + "\n   " + "[B] " + b + "\n   " + "[C] " + c + "\n   " + "[D] " + d ;
}


string updateQuestion(int&index){
    string question;
    string a;
    string b;
    string c;
    string d;

    cout << "Question :" ;
    cin >> question;
    cout << "[A] :";
    cin >> a;
    cout << "[B] :";
    cin >> b;
    cout << "[C] :";
    cin >> c;
    cout << "[D] :";
    cin >> d;


    return (to_string(index)) + ". "+question+ "\n   " + "[A] " + a + "\n   " + "[B] " + b + "\n   " + "[C] " + c + "\n   " + "[D] " + d ;
}
char update_correct_answer(){
    char correct;
    cout << "Correct Answer : ";
    cin >> correct;
    return  correct;
}


void modifyeasyquestion(){
        char opt;
        int num;
        cout
         << "1. Update" << endl
         << "2. Delete" << endl
         << "3. Add" << endl
         << "4. Clear" << endl
         << "5. Exit" << endl;
        cout << " >> ";
        cin >> opt;
        if(opt == '1'){
            cout << "Enter Question number :";
            cin >> num;
            cout << endl;
            easyquestion[num-1] = updateQuestion(num);
            easycorrectansweruppercase[num-1] = update_correct_answer();
        }else if(opt == '2'){
        int num_easyquestions = 6;
        cout << "Enter Question number :";
        cin >> num;
        cout << endl;
        int newNum = 1;
        string newList[] = {};

         for (string&qs : easyquestion) {
             if(newNum >= num){
                char ch = static_cast<char>(newNum);
                easyquestion[newNum][0] = ' ';
                easyquestion[newNum] = to_string(newNum) + easyquestion[newNum];
                easyquestion[newNum-1] = easyquestion[newNum];
                easycorrectansweruppercase[newNum-1] = easycorrectansweruppercase[newNum];
                easycorrectanswerlowercase[newNum-1] = easycorrectanswerlowercase[newNum];
             }
            newNum++;
         }
            easyquestion[easyL-1] = " ";
            easycorrectansweruppercase[easyL-1] = ' ';
            easycorrectanswerlowercase[easyL-1] = ' ' ;
            easyL--;

    }else if(opt == '3'){
        int number = easyL +1;
        easyquestion[easyL] = add_new_element(number);
        easycorrectansweruppercase[easyL] = update_correct_answer();
        easyL++;
    }else if(opt == '4'){
        clearArray('e');
    }else if(opt == '5'){

    }
}

void modifymediumquestion(){
         char opt;
        int num;
        cout
         << "1. Update" << endl
         << "2. Delete" << endl
         << "3. Add" << endl
         << "4. Clear" << endl
         << "5. Exit" << endl;
        cout << " >> ";
        cin >> opt;
        if(opt == '1'){
            cout << "Enter Question number :";
            cin >> num;
            cout << endl;
            mediumquestion[num-1] = updateQuestion(num);
            mediumcorrectansweruppercase[num-1] = update_correct_answer();
        }else if(opt == '2'){
        int num_easyquestions = 6;
        cout << "Enter Question number :";
        cin >> num;
        cout << endl;
        int newNum = 1;
        string newList[] = {};
        /*
        if (num >= 1 && num <= num_easyquestions) {
            // Shift elements to fill the gap
            for (string&qs : easyquestion) {

                char ch = static_cast<char>(newNum);
                easyquestion[newNum][0] = ' ';
                easyquestion[newNum] = to_string(newNum) + easyquestion[newNum];
                easyquestion[newNum-1] = easyquestion[newNum];
                easycorrectansweruppercase[newNum-1] = easycorrectansweruppercase[newNum];
                easycorrectanswerlowercase[newNum-1] = easycorrectanswerlowercase[newNum];
                newNum++;

            }
            easyquestion[newNum-1][0] = ' ';
            easyquestion[newNum-1] = to_string(newNum) + easyquestion[newNum-1];

            num_easyquestions--;
        } else {
            cout << "Invalid question number." << endl;
        }*/
         for (string&qs : mediumquestion) {
             if(newNum >= num){
                char ch = static_cast<char>(newNum);
                mediumquestion[newNum][0] = ' ';
                mediumquestion[newNum] = to_string(newNum) + mediumquestion[newNum];
                mediumquestion[newNum-1] = mediumquestion[newNum];
                mediumcorrectansweruppercase[newNum-1] = mediumcorrectansweruppercase[newNum];
                mediumcorrectanswerlowercase[newNum-1] = mediumcorrectanswerlowercase[newNum];
                //cout << "**********" << newNum-1 << " = " <<newNum<< endl;
             }
            newNum++;
         }
            mediumquestion[mediumL-1] = " ";
            mediumcorrectansweruppercase[mediumL-1] = ' ';
            mediumcorrectanswerlowercase[mediumL-1] = ' ' ;
            mediumL--;

    }else if(opt == '3'){

    }
}

void modifyhardquestion(){
         char opt;
        int num;
        cout
         << "1. Update" << endl
         << "2. Delete" << endl
         << "3. Add" << endl
         << "4. Clear" << endl
         << "5. Exit" << endl;
        cout << " >> ";
        cin >> opt;
        if(opt == '1'){
            cout << "Enter Question number :";
            cin >> num;
            cout << endl;
            hardquestion[num-1] = updateQuestion(num);
            hardcorrectansweruppercase[num-1] = update_correct_answer();
        }else if(opt == '2'){
        int num_easyquestions = 6;
        cout << "Enter Question number :";
        cin >> num;
        cout << endl;
        int newNum = 1;
        string newList[] = {};

         for (string&qs : hardquestion) {
             if(newNum >= num){
                char ch = static_cast<char>(newNum);
                hardquestion[newNum][0] = ' ';
                hardquestion[newNum] = to_string(newNum) + hardquestion[newNum];
                hardquestion[newNum-1] = hardquestion[newNum];
                hardcorrectansweruppercase[newNum-1] = hardcorrectansweruppercase[newNum];
                hardcorrectanswerlowercase[newNum-1] = hardcorrectanswerlowercase[newNum];
                //cout << "**********" << newNum-1 << " = " <<newNum<< endl;
             }
            newNum++;
         }
            hardquestion[hardL-1] = " ";
            hardcorrectansweruppercase[hardL-1] = ' ';
            hardcorrectanswerlowercase[hardL-1] = ' ' ;
            hardL--;

    }else if(opt == '3'){

    }
}
int displayQuestions(){
     int index= 0;
    while(true){
    index = 0;
    char opt;
    cout << "Questions" << endl
         << "1. Easy Questions" << endl
         << "2. Medium Questions" << endl
         << "3. Hard Questions" << endl
         << "4. Exit" << endl;
    cout << " >> ";
    cin >> opt;
    switch(opt){
    case '1':
        opt = 'x';
        cout << "Easy Questions \n"<< endl;
        if(easyL != 0){
           
            for (string&ques : easyquestion){
            if(easyL <= index){

            }else{
                if(ques != ""){
                        cout << ques << endl;
                        cout << "Answer : " << easycorrectansweruppercase[index] << endl;
                }
            }
            index++;
        }
        }else{
            cout << "empty\n";
        }
        modifyeasyquestion();
        break;
    case '2':
        cout << "Medium Questions \n\n"<< endl;
        for (string&ques : mediumquestion){
            if(mediumL <= index){

            }else{
            cout << ques << endl;
            cout << "Answer : " << mediumcorrectansweruppercase[index] << endl;
            }

            index++;

        }
        modifymediumquestion();
        break;
    case '3':
        cout << "Hard Questions \n\n"<< endl;
        for (string&ques : hardquestion){
            if(hardL <= index){

            }else{
            cout << ques << endl;
            cout << "Answer : " << hardcorrectansweruppercase[index] << endl;
            }

            index++;

        }
        modifyhardquestion();
        break;
    case '4':
        return 0;
        break;
    default:
        cout << "Invalid Option !" << endl;
        break;
    }
    }


}
void saveRecord(string name, int score, string remarks) {
    ofstream file("C:/Users/Admin/quiz_records.txt", ios::app);
    if (file.is_open()) {
        file << "Name: " << name << "\n";
        file << "Score: " << score << "\n";
        file << "Remarks: " << remarks << "\n";
        file << "Date and Time: " << getTimestamp() << "\n";
        file << "------------------------\n";
        file.close();
    } else {
        cout << "Unable to open file for saving records." << endl;
    }
}

string studentcreateaccount(){
	string studentfirstname;
	string studentlastname;
	string studentcourse;
	int studentyearlevel;
	string studentpassword;

	cout << "CREATE ACCOUNT" << endl;
	cout << "\n\n";
	cout << "Enter First Name: ";
	cin >> studentfirstname;
	cout << "Enter Last Name: ";
	cin >> studentlastname;
	cout << "Enter Course: ";
	cin >> studentcourse;
	while(true){
        cout << "Enter Year Level([1] 1st year [2] 2nd year [3] 3rd year): ";
        cin >> studentyearlevel;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number." << endl;
        } else if (studentyearlevel > 3 || studentyearlevel <= 0) {
            cout << "Invalid year level!" << endl;
        } else {
            break;
        }

	}

	cout << "Enter Password: ";
	cin >> studentpassword;

	for (int x = 0; x < maxnum; x++)
	{
		if (Lastname[x] == "")
		{
			Lastname[x] = studentlastname;
			Firstname[x] = studentfirstname;
			Course[x] = studentcourse;
			Yearlevel[x] = studentyearlevel;
			Password[x] = studentpassword;
			counter++;
			cout << "Account Created Successfully!" << endl;

			return intostring(studentyearlevel);
		}
	}

}
bool tF = false;
void studentlogin(){
	tF = false;
	string lastname, password;
	cout << "LOGIN" << endl;
	cout << "\n\n";
	cout << "Enter Last Name: ";
	cin >> lastname;
	cout << "Enter Password: ";
	cin >> password;

	bool accfound = false;
	bool lastnamefound = false;
	bool passwordmatch = false;

	for(int i = 0; i < maxnum; i++){
		if(Lastname[i] == lastname){
			accfound = true;
			lastnamefound = true;
			if (Password[i] == password){
				passwordmatch = true;
				cout << "Login Successful!" << endl;
				activeUser = i;
				tF = true;
				break;
			}
		}
	}
	if(!lastnamefound && !passwordmatch){
		cout << "INVALID LASTNAME AND PASSWORD!" << endl;
	}
	else if(!lastnamefound){
			cout << "INVALID LASTNAME!" << endl;
	}
	else if (!passwordmatch){
		cout << "INVALID PASSWORD!";
	}
}


void checkstudentinfo(){

	if(counter != 0 || counter > 0){
            cout << "STUDENT RECORDS" << endl;
        for(int i = 0; i < counter; i++){
		cout << Lastname[i] << "\t\t" << Firstname[i] << "\t\t" << Course[i] << "\t\t" << Yearlevel[i] << "\t\t" << scores[i];
		if(quiztaken[i]){
			cout << "\t\t" << retakescores[i];
		}
		else{
			cout << "\t\tN/A";
		}
		cout << "\n";
	}
	}else{
	cout << "NO STUDENT RECORDS Found" << endl;
	}
	cout << endl;
}

int searchstudentrecord(string LastName){
	bool found = false;
	for(int i = 0; i < maxnum; i++){
		if(Lastname[i] == LastName){
        cout << "\nStudent Found!" << endl;
		cout << "Name: " << Lastname[i] << " " << Firstname[i] << endl;
		cout << "Course: " << Course[i] << endl;
		cout << "Year Level:" << Yearlevel[i] << endl;
		cout << "Score: " << scores[i] << endl;
		cout << "Retake Score: " << retakescores[i] << endl;
		found = true;
		return 0;
		}
	}

		cout << "\nStudent not Found!\n" << endl;
		found  = false;

}

void allQuestions(string questions[], int idx, char correctAnswersUpperCase[], char correctAnswerLowerCase[], int numQuestions, int activeUser, int& answered) {
    cout << questions[idx] << endl;
    cout << "Answer[A|B|C|D]: ";
    char answer;
    cin >> answer;
    if(answer == correctAnswersUpperCase[idx] || answer == correctAnswerLowerCase[idx]){
        cout << "Correct!" << endl;
        if(quiztaken[activeUser]){
            retakescores[activeUser]+=1;
            isAnswered = true;
        }
        else{
            scores[activeUser]+= 1;
            isAnswered = true;
        }
    }
    else if(answer != correctAnswersUpperCase[idx] || answer != correctAnswerLowerCase[idx]){
        cout << "Incorrect!" << endl;
        isAnswered = true;
    }else{
         cout << "\nTime's up! Moving to the next question...\n";
        }
    answered = 1;

}



void takequiz(string questions[], char correctAnswersUpperCase[], char correctAnswersLowerCase[], int numQuestions, int activeUser, int TIME_LIMIT) {
    for (int i = 0; i < numQuestions; i++) {
        int answered = 0;
        isAnswered = false;
        thread t(allQuestions, questions, i, correctAnswersUpperCase, correctAnswersLowerCase, numQuestions, activeUser, ref(answered));
        t.detach();

        int time_elapsed = 0;
        while (!answered && time_elapsed < TIME_LIMIT) {
            this_thread::sleep_for(chrono::seconds(1));
            time_elapsed++;
        }


        if (!answered) {
            cout << endl;
            continue;
        }

        cout << endl;
    }
}



void determinequizlevel(int studentyearlevel, int studentindex){
	if(studentyearlevel == 1){
		takequiz(easyquestion, easycorrectansweruppercase, easycorrectanswerlowercase,  sizeof(easyquestion) / sizeof(easyquestion[0]), studentindex, 10);
	}
	else if(studentyearlevel == 2){
		takequiz(mediumquestion, mediumcorrectansweruppercase, mediumcorrectanswerlowercase,  sizeof(mediumquestion) / sizeof(mediumquestion[0]), studentindex, 8);
	}
	else if(studentyearlevel == 3){
		takequiz(hardquestion, hardcorrectansweruppercase, hardcorrectanswerlowercase,  sizeof(hardquestion) / sizeof(hardquestion[0]), studentindex, 5);
	}
	else{
		cout << "Invalid Year Level!" << endl;
	}
}


void takequiz(string questions[], char correctAnswersUpperCase[], char correctAnswerLowerCase[], int numQuestions, int studentindex, string name){
    int score = 0;
    int TIME_LIMIT = 5;
    for(int i = 0; i < numQuestions; i++){
        cout << questions[i] << endl;
        cout << "Answer[A|B|C|D]: ";
        cin >> answer[i];

        auto start_time = chrono::steady_clock::now(); // Start the timer

        bool answered = false;
        while (!answered) {
            auto end_time = chrono::steady_clock::now(); // Get current time
            auto elapsed_time = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

            if (elapsed_time >= 1) {
                cout << "Time's up!" << endl;
                break;
            }

            // Check if the user has entered the answer
            if (answer[i] == correctAnswersUpperCase[i] || answer[i] == correctAnswerLowerCase[i]) {
                answered = true;
                if(answer[i] == correctAnswersUpperCase[i]){
                    cout << "Correct!" << endl;
                    score++;
                } else {
                    cout << "Incorrect!" << endl;
                }
            }
        }

        if (!quiztaken[studentindex]) {
            scores[studentindex] = score;
        } else {
            retakescores[studentindex] = score;
        }
        quiztaken[studentindex] = true;
    }
}

int quiz(){
                            int option;
                            cout << "\n\n[1] TAKE QUIZ" << endl;
                            cout << "[2] LOGOUT"<< endl;
                            cout << "\n\n";
                            cout<< " >> ";
                            cin >> option;

                                switch (option) {
                                    case 1:
                                        if (counter > 0 && !quiztaken[activeUser]) {
                                            determinequizlevel(Yearlevel[activeUser], activeUser);
                                            if (counter > 0) {
                                                cout << "Last Name: " << Lastname[activeUser] << "\t\t" << "Score: " << scores[activeUser] << endl;
                                                quiztaken[activeUser] = true;
                                                string remarks = (scores[activeUser] >= 4) ? "PASSED" : "FAILED";
                                                saveRecord(Firstname[activeUser] + " " + Lastname[activeUser], scores[activeUser], remarks);
                                                leaderboard();
                                                quiz();
                                            }
                                        } else if (counter > 0 && quiztaken[activeUser]) {
                                            cout << "Do you want to retake the quiz? [Y/N]: ";
                                            char retakechoice;
                                            cin >> retakechoice;

                                            if (retakechoice == 'Y' || retakechoice == 'y') {
                                                determinequizlevel(Yearlevel[activeUser], activeUser);
                                                cout << "Last Name: " << Lastname[activeUser] << "\t\t" << "Score: " << retakescores[activeUser] << endl;
                                                string remarks = (scores[activeUser] >= 4) ? "PASSED" : "FAILED";
                                                saveRecord(Firstname[activeUser] + " " + Lastname[activeUser], scores[activeUser], remarks);
                                                leaderboard();
                                                 quiz();
                                            }

                                        }
                                        break;
                                    case 2:
                                        return 0;
                                        break;
                                    default:
                                        cout << "\n\n";
                                        cout << "INVALID INPUT!";

                                        quiz();
                                        break;
                                }



}
bool isNumber(const string& str) {
    for (char const &c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}
int app(){

    cout << endl;
    std::cout << "				+--------------------------------------------------+" << std::endl;
    std::cout  << "				|               WELCOME TO QUIZBY                  |"  << std::endl;
    std::cout << "				+--------------------------------------------------+" << std::endl;

    string adminusername, searchlastname, name;
    int adminrole, studentrole, option, adminpassword;
    string aUsername[3] = {"Austine", "Eunice", "Nest"};


        int role;
        string input;
        while (true) {
        cout << "\n\n";
        cout << "[1] ADMIN" << endl;
        cout << "[2] STUDENT" << endl;
        cout << "[3] EXIT" << endl;
        cout << "\n\n";
        cout << " >> ";
        cin >> input;

        if (isNumber(input)) {
            role = stoi(input);
            if (role >= 1 && role <= 3) {
                break;
            } else {
                cout << "Invalid option. Please enter a number between 1 and 3." << endl;
            }
        } else {
            cout << "Invalid input. Please enter a valid number." << endl;
        }
    }


        switch (role) {
            case 1:
                cout << "\n\n" << "ADMIN" << endl;
                cout << "Enter Username: ";
                cin >> adminusername;
                cout << "Enter Pin: ";
                cin >> adminpassword;
                if (adminpassword > 9999) {
                    cout << "You've exceeded the max pin. Please try again!" << endl;
                    return 1;
                }
                for (const string& names : aUsername) {
                    if (adminusername == names && adminpassword == 1111) {
                        do {
                            cout << "[1] CHECK STUDENT RECORDS" << endl;
                            cout << "[2] SEARCH STUDENT" << endl;
                            cout << "[3] QUESTIONS" << endl;
                            cout<< "[4] BACK" << endl;
                            cout << "\n\n";
                            cout << " >> ";
                            cin >> adminrole;

                            switch (adminrole) {
                                case 1:
                                    checkstudentinfo();
                                    break;
                                case 2:
                                    cout << "Enter the Last Name of the student to search: ";
                                    cin >> searchlastname;
                                    searchstudentrecord(searchlastname);
                                    break;
                                case 3:
                                    displayQuestions();
                                case 4:

                                    cout << "Back to main menu." << endl;
                                    adminrole = 5;
                                    sleep(1);
                                    break;
                                default:
                                    cout << "\n\n";
                                    cout << "INVALID INPUT!";
                                    break;
                            }
                        } while (adminrole != 5);
                    }
                    if(adminusername != names && adminpassword != 1111) {
                    	cout << "Wrong name/password. Please try again!" << endl;
                    	break;
					}
                }
                break;
            case 2:
                do {
                    cout << "\n\n" << "STUDENT" << endl;
                    cout << "[1] CREATE ACCOUNT" << endl;
                    cout << "[2] LOGIN" << endl;
                    cout << "[3] BACK" << endl;
                    cout << "\n\n";
                    cout << " >> ";
                    cin >> studentrole;

                    switch (studentrole) {
                        case 1:
                            name = studentcreateaccount();
                            sleep(1);
                            break;
                        case 2:
                            studentlogin();
                            if(tF == true) {
                            quiz();
                    }
                            break;
                        case 3:
                            return 0;
                            break;
                        default:
                            cout << "\n\n";
                            cout << "INVALID INPUT!";
                            break;
                    }
                } while (studentrole != 3);
                break;
            case 3:
                abort();
            default:
                cout << "\n\n";
                cout << "INVALID!\n\n"
                ;
                break;
        }

}
bool compareUsers(const User& a, const User& b) {
    return a.score > b.score;
}

void sortleaderboard(vector<User>& users) {
    sort(users.begin(), users.end(), compareUsers);

    cout << "\n\n\n\nLeaderboard" << endl;
    cout << left << setw(15) << "Last Name"
         << left << setw(15) << "First Name"
         << left << setw(15) << "Course"
         << left << setw(15) << "Year Level"
         << left << setw(15) << "Score"
         << left << setw(15) << "Retake Score"
         << left << setw(15) << "Remarks" << endl;
    cout << "--------------------------------------------------------------------------------------------\n";
    int remarks = 1;
    for (const auto& user : users) {
        cout << left << setw(15) << user.lastName
             << left << setw(15) << user.firstName
             << left << setw(15) << user.course
             << left << setw(15) << user.yearLevel
             << left << setw(15) << user.score;
        if (user.quizTaken) {
            cout << left << setw(15) << user.retakeScore;
        } else {
            cout << left << setw(15) << "N/A";
        }
        cout << left << setw(15) << remarks ;
        remarks++;
        cout << "\n";
    }
    cout << endl;
}
void leaderboard(){
    vector<User> users;

	for(int i = 0; i < counter; i++){
		users.push_back({Lastname[i],Firstname[i],Course[i], Yearlevel[i],scores[i],retakescores[i],quiztaken[i]});
	}
    sortleaderboard(users);
}

int main() {
	while(true){
        app();
	}

    return 0;
}
