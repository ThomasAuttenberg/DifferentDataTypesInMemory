

#include <iostream>

#include <vector>
#include "Primitive.h"
#include <fstream>
#include <string>
#include <typeinfo>
#include <sstream>
#include <list>
#include "clist.h"
#include "Menu.h"
#include <conio.h>
#include <chrono>
#include <algorithm>


template <class T>
void safeInput(T& value) {
	
	while (!(std::cin >> value)) {
			std::cout << "\033[01;38;05;15;48;05;196mНекорректный ввод. Повторите попытку\033[0m\n";
			std::cin.clear();
			std::cin.ignore(32767, '\n');
;		}
	std::cin.ignore(32767, '\n');

}

int main()
{
	system("chcp 1251");
	system("cls");


	clist<Object*> list;
	clist<Object*>::iterator iter;
	clist<Object*>::iterator iter_fixed;
	bool fixedIterActive = false;

	Menu navigationMenu([&]() {

		std::cout <<
			" ==================================================================\n\n"
			<< " Используйте стрелки, чтобы переключаться между пунктами меню\n"
			<< " Используйте TAB чтобы зафиксировать итератор и создать новый\n"
			<< "\n ==================================================================\n\n"
			<< (fixedIterActive ? "\033[1;32m Итератор зафиксирован как \033[4mитератор начала\033[0m\033[1;32m. Перемещайте новый итератор \033[0m\n\n" : "")
			<< " Текущее содержимое циклического списка: ";

		list.foreach([&](Object* ptr) {
			std::cout << " ";

			if (fixedIterActive && *iter_fixed == ptr) std::cout << "\033[1;37;46m";
			if (*iter == ptr) std::cout << "\033[1;37;42m";
			if (ptr->identifier() == Float::identifier) std::cout << *((Float*)(ptr));
			if (ptr->identifier() == Int::identifier) std::cout << *((Int*)(ptr));
			if (ptr->identifier() == Char::identifier) std::cout << *((Char*)(ptr));
			std::cout << "\033[0m";
			});
		});

	navigationMenu.addKeyListener(75, [&iter]() {
		try {
			iter--;
		}
		catch (std::exception ex) {
			std::cout << "\n\n Поймано исключение: " << ex.what();
			_getch();
		}
		}, true);

	navigationMenu.addKeyListener(77, [&iter]() {
		try {
			iter++;
		}
		catch (std::exception ex) {
			std::cout << "\n\n Поймано исключение: " << ex.what();
			_getch();
		}
		}, true);

	navigationMenu.addKeyListener(9, [&]() {
		if (fixedIterActive) fixedIterActive = false;
		else try {
			*iter;
			iter_fixed = iter;
			fixedIterActive = true;
		}
		catch (std::exception ex) {
			std::cout << "\n\n Первого итератора не существует! Заполните список";
			_getch();
		}
		}, false);

	enum INSERT_TYPE {
		FLOAT, CHAR, INT
	};
	enum INSERT_METHOD {
		PUSH_BACK, PUSH_FRONT, ITERATOR
	};
	INSERT_TYPE insertType;
	INSERT_METHOD insertMethod;
	iter = list.begin();

	Menu InputPage([&]() {

		std::cout << "Введите значение для элемента: ";
		if (insertType == INSERT_TYPE::CHAR) {
			char inputBuffer;
			safeInput(inputBuffer);
			if (insertMethod == INSERT_METHOD::PUSH_BACK)
				list.push_back((new Char(inputBuffer)));
			if (insertMethod == INSERT_METHOD::PUSH_FRONT)
				list.push_front((new Char(inputBuffer)));
			if (insertMethod == INSERT_METHOD::ITERATOR)
				list.insert(iter, new Char(inputBuffer));
		}
		if (insertType == INSERT_TYPE::INT) {
			int inputBuffer;
			safeInput(inputBuffer);
			if (insertMethod == INSERT_METHOD::PUSH_BACK)
				list.push_back((new Int(inputBuffer)));
			if (insertMethod == INSERT_METHOD::PUSH_FRONT)
				list.push_front((new Int(inputBuffer)));
			if (insertMethod == INSERT_METHOD::ITERATOR)
				list.insert(iter, new Int(inputBuffer));
		}
		if (insertType == INSERT_TYPE::FLOAT) {
			float inputBuffer;
			safeInput(inputBuffer);
			if (insertMethod == INSERT_METHOD::PUSH_BACK)
				list.push_back((new Float(inputBuffer)));
			if (insertMethod == INSERT_METHOD::PUSH_FRONT)
				list.push_front((new Float(inputBuffer)));
			if (insertMethod == INSERT_METHOD::ITERATOR)
				list.insert(iter, new Float(inputBuffer));
		}
		Menu::console.setMenu(&navigationMenu);

		if (list.size() == 1) iter = list.begin();

		}, true);
	std::list<Object*> allocatedObjects;
	Menu readFilePage([&]() {
		std::cout << "\n\n Введите имя файла или break для отмены:";
		std::string fileNameUserInput;
		std::ifstream inputStream;
		while (true) {
			std::cout << "\n input > ";
			std::cin >> fileNameUserInput;
			if (fileNameUserInput == "break") return;
			inputStream.open(fileNameUserInput);
			if (inputStream.is_open()) {
				break;
			}
			else {
				std::cout << "\033[01;38;05;15;48;05;196m\nНекорректное имя файла или файл не может быть открыт для чтения. Повторите попытку или введите break для прекращения операции\n\033[0m";
				continue;
			}
		}
		bool wasListEmpty = !list.size();
		while (true) {
			size_t type;
			inputStream.read((char*)&type, sizeof(size_t));
			if (inputStream.eof()) break;
			if (type == Float::identifier) {
				Float* obj = new Float;
				(*obj).readBinary(inputStream);
				list.push_back(obj);
			}
			if (type == Int::identifier) {
				Int* obj = new Int;
				(*obj).readBinary(inputStream);
				list.push_back(obj);
			}
			if (type == Char::identifier) {
				Char* obj = new Char;
				(*obj).readBinary(inputStream);
				list.push_back(obj);
			}
		}
		if (wasListEmpty) iter = list.begin();

		}, true);

	Menu writeFilePage([&]() {
		std::cout << "\n\n Введите имя файла или break для отмены:";
		std::string fileNameUserInput;
		std::ofstream outputStream;
		while (true) {
			std::cout << "\n input > ";
			std::cin >> fileNameUserInput;
			if (fileNameUserInput == "break") return;
			outputStream.open(fileNameUserInput);
			if (outputStream.is_open()) {
				break;
			}
			else {
				std::cout << "\033[01;38;05;15;48;05;196m\nНекорректное имя файла или файл не может быть открыт для записи. Повторите попытку или введите break для прекращения операции\n\033[0m";
				continue;
			}
		}
		list.foreach([&](Object* obj) {
			size_t identifier = obj->identifier();
			outputStream.write((char*)&identifier, sizeof(size_t));
			obj->writeBinary(outputStream);
			});
		}, true);

	Menu editValuePage([&]() {
		std::cout << " Введите новое значение для выбранного элемента [ ";
		const char* type;
		if ((*iter)->identifier() == Float::identifier) {
			std::cout << *((Float*)*iter) << " ]";
			std::cout << "\n [ Float ]: ";
			safeInput(*((Float*)*iter));
		}
		if ((*iter)->identifier() == Char::identifier) {
			std::cout << *((Char*)*iter) << " ]";
			std::cout << "\n [ Char ]: ";
			char newValue;
			safeInput(newValue);
			*((Char*)*iter) = newValue;
		}
		if ((*iter)->identifier() == Int::identifier) {
			std::cout << *((Int*)*iter) << " ]";
			std::cout << "\n [ Int ]: ";
			safeInput(*((Int*)*iter));
		}

		}, true);
	char letter = '?';

	Menu compareValuesPage([&]() {
		try {
			std::cout << "Выбранные объекты:\n";
			if ((*iter_fixed)->identifier() == Float::identifier) {
				std::cout << "    1) [ Float ]: " << *((Float*)*iter_fixed) << "\n";
			}
			if ((*iter_fixed)->identifier() == Char::identifier) {
				std::cout << "    1) [ Char ]: " << *((Char*)*iter_fixed) << "\n";
			}
			if ((*iter_fixed)->identifier() == Int::identifier) {
				std::cout << "    1) [ Int ]: " << *((Int*)*iter_fixed) << "\n";
			}
			if ((*iter)->identifier() == Float::identifier) {
				std::cout << "    2) [ Float ]: " << *((Float*)*iter) << "\n";
			}
			if ((*iter)->identifier() == Char::identifier) {
				std::cout << "    2) [ Char ]: " << *((Char*)*iter) << "\n";
			}
			if ((*iter)->identifier() == Int::identifier) {
				std::cout << "    2) [ Int ]: " << *((Int*)*iter) << "\n";
			}
			std::cout << "      Результат сравнения: \033[32m";
			std::partial_ordering ordering = (*iter_fixed)->compare(**iter);
			if (ordering == std::partial_ordering::equivalent) {
				std::cout << "Объекты равны";
				letter = 'e';
			}
			if (ordering == std::partial_ordering::greater) {
				std::cout << "Первый объект больше второго ";
				letter = 'g';
			}
			if (ordering == std::partial_ordering::less) {
				std::cout << "Первый объект меньше второго ";
				letter = 'l';
			}
			if (ordering == std::partial_ordering::unordered) {
				std::cout << "\033[33mНевозможно обеспечить корректное сравнение: unordered";
				letter = 'u';
			}
			std::cout << "\033[0m";
			std::cout << "\nequal = e, less = l, greater = g, unordered = u";
		}
		catch (std::exception ex) {
			std::cout << " \nПоймано исключение:  " << ex.what() << " \n Проверьте, что список не пустой, и созданы оба итератора";
		}

		});
	MenuItem compareValuesPageItem("Put result as e/l/g/u in list", [&] {
		list.push_back(new Char(letter));
		if (list.size() == 1) iter = list.begin();
		Menu::console.setMenu(&navigationMenu);
		});
	compareValuesPage.addItem(compareValuesPageItem);

	Object* folded = nullptr;
	bool wasPreviousPut = false;
	Menu foldValuesPage([&]() {
		if (!wasPreviousPut) delete folded;
		wasPreviousPut = false;
		folded = nullptr;
		std::cout << "Выбранные объекты:\n";
		if ((*iter_fixed)->identifier() == Float::identifier) {
			std::cout << "    1) [ Float ]: " << *((Float*)*iter_fixed) << "\n";
		}
		if ((*iter_fixed)->identifier() == Char::identifier) {
			std::cout << "    1) [ Char ]: " << *((Char*)*iter_fixed) << "\n";
		}
		if ((*iter_fixed)->identifier() == Int::identifier) {
			std::cout << "    1) [ Int ]: " << *((Int*)*iter_fixed) << "\n";
		}
		if ((*iter)->identifier() == Float::identifier) {
			std::cout << "    2) [ Float ]: " << *((Float*)*iter) << "\n";
		}
		if ((*iter)->identifier() == Char::identifier) {
			std::cout << "    2) [ Char ]: " << *((Char*)*iter) << "\n";
		}
		if ((*iter)->identifier() == Int::identifier) {
			std::cout << "    2) [ Int ]: " << *((Int*)*iter) << "\n";
		}
		folded = (*iter_fixed)->getCopy();
		folded->add(**iter);
		char* result = folded->to_cstring();
		std::cout << "      Результат сложения: \033[32m" << result << "\033[0m";
		delete[] result;
		});
	MenuItem foldValuesPageItem("Put result in the list", [&]() {
		list.push_back(folded);
		Menu::console.setMenu(&navigationMenu);
		wasPreviousPut = true;
		});
	foldValuesPage.addItem(foldValuesPageItem);

	Menu insertingPage("Выберите тип вставляемого элемента");

	MenuItem insertingPage_i1("Float", [&]() {
		insertType = INSERT_TYPE::FLOAT;
		Menu::console.setMenu(&InputPage);
		});
	MenuItem insertingPage_i2("Char", [&]() {
		insertType = INSERT_TYPE::CHAR;
		Menu::console.setMenu(&InputPage);
		});
	MenuItem insertingPage_i3("Int", [&]() {
		insertType = INSERT_TYPE::INT;
		Menu::console.setMenu(&InputPage);
		});
	insertingPage.addItem(insertingPage_i1);
	insertingPage.addItem(insertingPage_i2);
	insertingPage.addItem(insertingPage_i3);

	Menu testingPage([&]() {
		clist<Object*> testList;
		system("cls");
		std::cout
			<< "=================================================\n"
			<< " Страница тестирования циклического списка\n"
			<< " Введите бинарный файл для загрузки содержимого списка или break для отмены:";
		std::string dumpFile;
		std::ifstream dumpStream;
		while (true) {
			std::cout << "\n > ";
			std::cin >> dumpFile;
			dumpStream.open(dumpFile);
			if (dumpStream.is_open() || dumpFile == "break") {
				break;
			}
			else {
				std::cout << "\033[33m// указанный файл не может быть открыт для чтения.\033[0m";
			}
		}
		if (dumpFile == "break") {
			return;
		}

		while (true) {
			int type;
			dumpStream.read((char*)&type, sizeof(int));
			if (dumpStream.eof()) break;
			if (type == Float::identifier) {
				Float* obj = new Float;
				(*obj).readBinary(dumpStream);
				testList.push_back(obj);
			}
			if (type == Int::identifier) {
				Int* obj = new Int;
				(*obj).readBinary(dumpStream);
				testList.push_back(obj);
			}
			if (type == Char::identifier) {
				Char* obj = new Char;
				(*obj).readBinary(dumpStream);
				testList.push_back(obj);
			}
		}
		std::cout
			<< "=================================================\n"
			<< " Файл прочитан. Количество объектов в списке: "
			<< testList.size()
			<< " Проводим тестирование...\n"
			<< "=================================================\n";
		if (testList.size() < 10) std::cout << " \n Тестирование прекращено. Циклический список содержит слишком мало элементов (<10):";
		// Test 1: Перемещение итератора
		const int max_element_iter_moves_to = 4000;
		std::chrono::nanoseconds iteratorMovingTime[max_element_iter_moves_to];
		std::chrono::steady_clock::time_point operationStart;
		std::chrono::steady_clock::time_point operationEnd;
		int min_of_two_requirements = max_element_iter_moves_to < testList.size() ? max_element_iter_moves_to : testList.size();
		std::ofstream ofs;
		ofs.open("testResults.txt");
		ofs << "Количество элементов: " << testList.size();
		ofs << "\n\nВзятие по индексу\n";
		ofs << "index | Время [мкс]\n";

		for (int i = 1; i < max_element_iter_moves_to && i < testList.size(); i++) {
			operationStart = std::chrono::steady_clock::now();
			auto iterator = testList.begin();
			if (i > testList.size() / 2) {
				for (int j = 0; j <= testList.size() - i; j++) {
					iterator--;
				}
			}
			else {
				for (int j = 0; j <= i; j++) {
					iterator++;
				}
			}
			*iterator;
			operationEnd = std::chrono::steady_clock::now();
			ofs << i << " " << std::chrono::duration_cast<std::chrono::microseconds>(operationEnd - operationStart).count() << "\n";
		}
		ofs << "Поиск\n";
		ofs << "index искомого | Время [мкс]\n";
		for (int i = 1; i < max_element_iter_moves_to && i < testList.size(); i++) {
			auto iterator = testList.begin();
			operationStart = std::chrono::steady_clock::now();
			for (int j = 0; j <= i; j++) {
				iterator++;
				*iterator == nullptr;
			}
			operationEnd = std::chrono::steady_clock::now();
			ofs << i << " " << std::chrono::duration_cast<std::chrono::microseconds>(operationEnd - operationStart).count() << "\n";
		}

		ofs << "\n\nВставка в начало [нс]: ";
		Int* newElem;
		newElem = new Int(1);
		operationStart = std::chrono::steady_clock::now();
		testList.push_front(newElem);
		operationEnd = std::chrono::steady_clock::now();
		ofs << std::chrono::duration_cast<std::chrono::nanoseconds>(operationEnd - operationStart).count();
		ofs << " | Вставка в конец [нс]: ";
		newElem = new Int(1);
		operationStart = std::chrono::steady_clock::now();
		testList.push_back(newElem);
		operationEnd = std::chrono::steady_clock::now();
		ofs << std::chrono::duration_cast<std::chrono::nanoseconds>(operationEnd - operationStart).count();
		ofs << "\nУдаление из начала [нс]: ";
		delete testList.front();
		operationStart = std::chrono::steady_clock::now();
		testList.pop_front();
		operationEnd = std::chrono::steady_clock::now();
		ofs << std::chrono::duration_cast<std::chrono::nanoseconds>(operationEnd - operationStart).count();
		ofs << "| Удаление из конца [нс]: ";
		delete testList.back();
		operationStart = std::chrono::steady_clock::now();
		testList.pop_back();
		operationEnd = std::chrono::steady_clock::now();
		ofs << std::chrono::duration_cast<std::chrono::nanoseconds>(operationEnd - operationStart).count();
		ofs.close();
		testList.foreach([](Object* item) {
			delete item;
			});
		testList.clear();
		std::cout << "\n\n Тестирование завершено. Результаты записаны в файл testResults.txt";
		_getch();
		}, true);

	MenuItem nav_i1("insert", [&]() {
		insertMethod = INSERT_METHOD::ITERATOR;
		Menu::console.setMenu(&insertingPage);
		});
	MenuItem nav_i2("erase", [&]() {
		try {
			if (fixedIterActive) {
				auto iter_ = iter_fixed;
				while (iter_ != iter) {
					delete* iter_;
					iter_++;
				}
				list.erase(iter_fixed, iter);
				fixedIterActive = false;
			}
			else {
				delete* iter;
				list.erase(iter);
			}
			iter = list.begin();
		}
		catch (std::exception ex) {
			std::cout << "\n\n Поймано исключение: " << ex.what();
			_getch();
		}
		iter = list.begin();
		});
	MenuItem nav_i3("push_front", [&]() {
		insertMethod = INSERT_METHOD::PUSH_FRONT;
		Menu::console.setMenu(&insertingPage);
		});
	MenuItem nav_i4("push_back", [&]() {
		insertMethod = INSERT_METHOD::PUSH_BACK;
		Menu::console.setMenu(&insertingPage);
		});
	MenuItem nav_i5("pop_front", [&]() {
		if (iter == list.begin()) {
			try {
				iter++;
			}
			catch (std::exception ex) {};
		}
		if (iter_fixed == list.begin()) fixedIterActive = false;
		try {
			list.pop_front();
			if (list.size() == 0) iter = list.begin();
		}
		catch (std::exception ex) {
			std::cout << "\n Поймано исключение: " << ex.what();
			_getch();
		}
		});
	MenuItem nav_i6("pop_back", [&]() {
		try {
			if (iter == --list.begin()) {
				iter--;
			}
			if (fixedIterActive && iter_fixed == --list.begin()) fixedIterActive = false;
		}
		catch (std::exception ex) {};
		try {
			list.pop_back();
			if (list.size() == 0) iter = list.begin();
		}
		catch (std::exception ex) {
			std::cout << "\n Поймано исключение: " << ex.what();
			_getch();
		}
		});
	MenuItem nav_i7("shift_forward", [&]() {
		try {
			list.shift_forward();
		}
		catch (std::exception ex) {
			std::cout << "\n Поймано исключение: " << ex.what();
			_getch();
		}
		});
	MenuItem nav_i8("shift_backward", [&]() {
		try {
			list.shift_backward();
		}
		catch (std::exception ex) {
			std::cout << "\n Поймано исключение: " << ex.what();
			_getch();
		}
		});
	MenuItem nav_i9("readFromBinaryFile", readFilePage);
	MenuItem nav_i10("writeToBinaryFile", writeFilePage);
	MenuItem nav_i11("\n[Edit Value]", editValuePage);
	MenuItem nav_i12("[Compare values]", compareValuesPage);
	MenuItem nav_i13("[Fold Values]", foldValuesPage);
	MenuItem nav_i14("[Tests]", testingPage);

	navigationMenu.addItem(nav_i1);
	navigationMenu.addItem(nav_i2);
	navigationMenu.addItem(nav_i3);
	navigationMenu.addItem(nav_i4);
	navigationMenu.addItem(nav_i5);
	navigationMenu.addItem(nav_i6);
	navigationMenu.addItem(nav_i7);
	navigationMenu.addItem(nav_i8);
	navigationMenu.addItem(nav_i9);
	navigationMenu.addItem(nav_i10);
	navigationMenu.addItem(nav_i11);
	navigationMenu.addItem(nav_i12);
	navigationMenu.addItem(nav_i13);
	navigationMenu.addItem(nav_i14);



	Menu::console.setMenu(&navigationMenu);
	Menu::console.show();

	
}
