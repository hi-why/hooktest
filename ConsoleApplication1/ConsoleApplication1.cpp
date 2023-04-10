// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "string"
#include <iostream>
using namespace std;
char note[] = "input a string:";
int main() {
	string str;	
	cout << note;
	cout << &note;
	getline(cin, str);
	cout << "你输入的字符串是：" << str << endl;
	return 0;
}