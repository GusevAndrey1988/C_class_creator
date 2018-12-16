/**
 * Утилита предназначена для создания c++ класса.
 * Создаёт фаил загаловка и если нужно фаил реализации класса.
 */

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <numeric>
#include <algorithm>

#define PATH_MAX 256

struct Config {
	using Super_class_list = std::vector<std::string>;

	bool create_source{true};
	bool create_namespace{false};
	std::string namespace_name;
	std::string class_name;
	Super_class_list super_class_list;	
	std::string include_dir;
	std::string source_dir;
};

/** 
 * @brief Возвращает путь рабочего каталога
 * @param none
 * @return Путь рабочего каталога
 */
std::string get_work_dir()
{
	char result[PATH_MAX];
	getcwd(result, PATH_MAX);
	return std::string(result);
}

/**
 * @brief Возвращает значение следующее за ключом
 * @param i Текущее половение в массиве
 * @param argc Число елементов массива
 * @param argv Массив
 * @return Значение ключа
 */
std::string get_value(int &i, const int &argc, char *argv[])
{
	// if i+1 < argc => has next value
	if (i+1 < argc && argv[i+1][0] != '-') {
		++i;
		return std::string{argv[i]};
	}

	return std::string{};
}

/**
 * @brief Возвращает значения следующие за ключом
 * @param i Текущее половение в массиве
 * @param argc Число елементов массива
 * @param argv Массив
 * @param values Значения ключа
 * @return none 
 */
void get_values_array(int &i, const int &argc, char *argv[],
		std::vector<std::string> &values)
{
	// if i+1 < argc => has next value
	while (i+1 < argc && argv[i+1][0] != '-') {
		++i;
		values.push_back(argv[i]);
	}
}

/**
 * @brief Выводит значения массива через запятую
 */
void print_str_values(const std::vector<std::string> &values) 
{
	for (std::vector<std::string>::size_type i = 0;
			i != values.size(); ++i) {

		if (i == 0) std::cout << values[i];
		else std::cout << ", " << values[i];
	}
}

/**
 * @brief Выводит заданные параметры
 */
void print_config(const Config &conf) 
{
	using namespace std;

	if (conf.class_name.size() > 0) {
		cout << "Class name: " << conf.class_name << endl;
	}
	if (conf.super_class_list.size() > 0) {
		cout << "Super classes: "; 
		print_str_values(conf.super_class_list); 
		cout << endl;
	}
	if (conf.create_namespace) {
		cout << "namespace " << conf.namespace_name << endl;
	}
	if (!conf.create_source) {
		cout << "no_source" << endl;
	}
	if (conf.include_dir.size() > 0) {
		cout << "inlude_dir: " << conf.include_dir << endl;
	}
	if (conf.source_dir.size() > 0) {
		cout << "source_dir: " << conf.source_dir << endl;
	}
}

/**
 * @brief Ошибка имени класса
 */
class class_name_error {  };

/**
 * @brief Создаёт имя файла
 * @param dir Директория файла
 * @param file_name Имя файла 
 * @param exe Расширение
 * @return Имя файла
 */
std::string create_file_name(const std::string &dir,
		const std::string &file_name, const std::string &exe)
{
	std::string ret_file_name = dir;
	if (dir.size() > 0)
		ret_file_name += '/';
	ret_file_name += file_name + exe; 

	return ret_file_name;
}

/**
 * @brief Создаёт имя файла заголовка по имени класса
 */
std::string create_header_file_name(const std::string &dir,
		const std::string &class_name) 
{
	return std::move(create_file_name(dir, class_name, ".h"));
}

/**
 * @brief Создаёт имя файла исходного кода по имени класса
 */
std::string create_source_file_name(const std::string &dir,
		const std::string &class_name) 
{
	return std::move(create_file_name(dir, class_name, ".cpp"));
}

/**
 * @brief Переводит исходную строку в верхний регистр (ASCII)
 * @return Строка в верхнем регистре
 */
std::string to_upper(const std::string &str)
{
	std::string ret_str = str;
	for (char &ch : ret_str) {
		if (ch < 0x7b && 0x60 < ch)
			ch = ch - 0x20;
	}

	return ret_str;
}

/**
 * @brief Создаёт текст файла загаловка 
 * @param Параметры создания класса
 * @return Текст файла загаловка
 */
std::string create_header_file_text(const Config &conf)
{
	using namespace std;

	ostringstream ret_text;

	string hdr_prtc_def_name{"_"};
	hdr_prtc_def_name += to_upper(conf.class_name) + "_H";

	ret_text 
		<< "#ifndef " << hdr_prtc_def_name << '\n'  
		<< "#define " << hdr_prtc_def_name << "\n\n";

	if (conf.create_namespace) {
		ret_text
			<< "namespace " << conf.namespace_name << " {\n\n"; 
	}

	ret_text << "class " << conf.class_name;
	if (conf.super_class_list.size() > 0) {
		ret_text << " : ";
		for (Config::Super_class_list::size_type i = 0;
				i != conf.super_class_list.size(); ++i) {

			if (i == 0) {
				ret_text << "public " << conf.super_class_list[i];
			}
			else {
				ret_text << ", public " << conf.super_class_list[i];
			}
		}
	}

	ret_text
	   	<< " {\nprivate:\npublic:\n}; //" << conf.class_name << "\n\n";

	if (conf.create_namespace) {
		ret_text << "} //" << conf.namespace_name << "\n\n";
	}

	ret_text << "#endif //" << hdr_prtc_def_name;

	return std::move(ret_text.str());
}

/**
 * @brief Создаёт текст файла исходного кода 
 * @param Параметры создания класса
 * @return Текст файла исходного кода 
 */
std::string create_source_file_text(const Config &conf) 
{
	using namespace std;

	ostringstream ret_text;
	int slash_cnt = 0;

	string source_file_name = create_source_file_name(
			conf.include_dir, conf.class_name);

	slash_cnt = count(source_file_name.begin(),
			source_file_name.end(), '/');

	string hdr_path("\"");
	for (int i = 0; i != slash_cnt; ++i) {
		hdr_path += "../";
	}

	hdr_path += create_header_file_name(
			conf.include_dir, conf.class_name) + "\"";

	ret_text << "#include " << hdr_path << "\n\n";

	if (conf.create_namespace) {
		ret_text << "namespace " << conf.namespace_name 
			<< " {\n\n} //" << conf.namespace_name;
	}

	return std::move(ret_text.str());
}

struct create_file_error {
	std::string what;

	create_file_error(const std::string &s) : what{s} {  }
};

/**
 * @brief Создаёт класс
 * @param Параметры создания класса
 */
void create_class(const Config &conf) 
{
	using namespace std;
	
	string path = get_work_dir();

	if (conf.class_name.size() == 0) throw class_name_error{};

	string header_file_name = std::move(create_header_file_name(
				conf.include_dir, conf.class_name));
	header_file_name = path + '/' + header_file_name;

	string source_file_name = std::move( create_source_file_name(
				conf.source_dir, conf.class_name));
	source_file_name = path + '/' + source_file_name;

	string header_file_text =
	   	std::move(create_header_file_text(conf));
	string source_file_text;
	if (conf.create_source)
		source_file_text = std::move(create_source_file_text(conf));

	ofstream header_file{header_file_name};
	if (!header_file) throw create_file_error{header_file_name};

	ofstream source_file;
	if (conf.create_source) {
		source_file.open(source_file_name);
		if (!source_file) {
			remove(header_file_name.c_str());
			throw create_file_error{source_file_name};
		}
	}

	header_file << header_file_text;
	if (conf.create_source)
		source_file << source_file_text;
}

void print_help()
{
	std::cout
		<< "\t-cn NAME            - Имя класса;\n"
		<< "\t-pc NAME1 NAME2 ... - Список суперклассов;\n"
		<< "\t-ns [NAME]          - Пространство имён;\n"
		<< "\t-id DIR             - Директория файлов заголовков;\n"
		<< "\t-sd DIR             - Директория файлов исходного кода;\n"
		<< "\t-nos                - Не создавать файл исходного кода;\n"
		<< "\t-h                  - Эта справка;\n"
		<< std::endl;
}

/* C_class_creator -cn NAME -id DIR -sd DIR -nos -ns NAME -pc NAME */
int main(int argc, char *argv[]) try
{
	using namespace std;

	Config conf;	

	for (int i = 1; i != argc; ++i) {
		string arg{argv[i]};

		if (arg == "-nos") {
			conf.create_source = false;
		} else if (arg == "-ns") {
			conf.create_namespace = true;
			conf.namespace_name = get_value(i, argc, argv);
		} else if (arg == "-pc") {
			get_values_array(i, argc, argv, conf.super_class_list);
		} else if (arg == "-cn") {
			conf.class_name = get_value(i, argc, argv);
		} else if (arg == "-id") {
			conf.include_dir = get_value(i, argc, argv);
		} else if (arg == "-sd") {
			conf.source_dir = get_value(i, argc, argv);
		} else if (arg == "-h") {
			print_help();

			return EXIT_SUCCESS;
		} else {
			cout << "-h for help" << endl;
			return EXIT_SUCCESS;
		}
	}

	if (argc == 1) {
	   	cout << "-h for help" << endl;
		return EXIT_SUCCESS;
	} else {
		print_config(conf);
		create_class(conf);
	}

	return EXIT_SUCCESS;
} catch (class_name_error) {
	std::cerr << "no class name" << std::endl;
	return EXIT_FAILURE;
} catch (create_file_error &e) {
	std::cerr << "can't create file: " << e.what << std::endl;
	return EXIT_FAILURE;
} catch (...) {
	std::cerr << "unknown error" << std::endl;
	return EXIT_FAILURE;
}
