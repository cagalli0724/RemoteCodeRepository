//////////////////////////////////////////////////////////////////////
// HideFuncHandle.cpp - demonstrate how to add div and span block to//
//                     hiden and unhiden the code blocks in webpages//
//  ver 1.0                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////


#include "HideFuncHandle.h"
#include "../Utilities/Utilities.h"

using namespace CodeAnalysis;

//modify the line content which should be added the button.
std::string HideFuncHandle::ModifyForDIVBn(int lineNum, std::string str, bool &ifModify, std::string intToStr)
{
	std::stringstream lgnToStr;
	int j = (str.length() * 12);
	for (int i = 0; i < (int)str.length(); i++) //caculate the margin-space for showing.
		if (str[i] == '\t')
			j = j + 90;
	lgnToStr << j;
	std::string temp = "<h4><span id=\"stateBut" + intToStr;
	temp = temp + "\" value=\"" + lgnToStr.str();
	temp = temp + "\" onClick=\"$use('div";
	temp = temp + intToStr + "','block" + intToStr + "','stateBut" + intToStr + "')\">-</span>";
	temp = temp + "&nbsp;" + str + "</h4>";
	temp = temp + "<h4><div class=\"class2\" id=\"block" + intToStr + "\">{...}</div></h4>";
	temp = temp + "<div class=\"class1\" id=\"div" + intToStr + "\">";
	return temp;
			
}
//if the str is ifdef, ifndef and endif, add div.
LineType HideFuncHandle::JudgeType(std::string str, int lineNum)
{
	bool ifShouldAddDiv = (((str.find("#ifndef") != std::string::npos) || (str.find("#ifdef") != std::string::npos))&& (str.find(";") == std::string::npos));

	if (ifShouldAddDiv)
	{
		return LineType::DEF_DIV;
	}

	ifShouldAddDiv =((str.find("#endif") != std::string::npos) && (str.find(";") == std::string::npos));

	if (ifShouldAddDiv)
	{
		return LineType::DEF_END;
	}
	if ((str.find("{") != std::string::npos) && (str.find("}") == std::string::npos))
			return LineType::DIV_BUTTON;

	if ((str.find("}") != std::string::npos) && (str.find("{") == std::string::npos))
			return LineType::DIV_END;

	int indexleft = str.find("}");
	int indexright = str.find("{");
	if ((indexleft != std::string::npos) && (indexright != std::string::npos) && (indexleft < indexright))
			return LineType::DIV_END;

	return LineType::NONE;

}
//modify the line content such as add div block, /div block and add span.
std::string HideFuncHandle::ModifyForHideFunc(int lineNum, std::string str, bool &ifModify)
{
	LineType ly = JudgeType(str, lineNum);
	int index, index2;
	std::stringstream intToStr;
	std::string subStr1, subStr2;
	intToStr << lineNum;
	switch (ly)
	{
	case LineType::DEF_DIV:
		return ModifyForDIVBn(lineNum, str, ifModify, intToStr.str());
		break;
	case LineType::DEF_END:
		return (str + "</div>");
		break;
	case LineType::DIV_BUTTON:
		index2 = str.find("\"{");    //if this condition isn't added, redundant div block will be added
		if (index2 != std::string::npos) {
			ifModify = false;
			return (str+"\n");
		}
		subStr1 = str.substr(0, str.find("{") + 1);
		subStr2 = str.substr(str.find("{")+1, str.length() -1);
		subStr1 = ModifyForDIVBn(lineNum, subStr1, ifModify, intToStr.str());//recursion
		return (subStr1 + ModifyForHideFunc(lineNum,subStr2, ifModify));
		break;
	case LineType::DIV_END:
		index = str.find("};"); //for class end.
		index2 = str.find("\"}");//if this condition isn't added, redundant div block will be added
		if (index2 != std::string::npos)   //cut the string.
		{
			ifModify = false;
			return (str+"\n");
		}
		if (index != std::string::npos) {
			subStr1 = str.substr(0, index + 2);
			subStr2 = str.substr(index + 2, str.length() - 1);
		}
		else {
			subStr1 = str.substr(0, str.find("}") + 1);
			subStr2 = str.substr(str.find("}") + 1, str.length() - 1);
		}
		subStr1 = subStr1 + "</div>";
		return (subStr1 + ModifyForHideFunc(lineNum, subStr2, ifModify));  //recursion
		break;
	default:
		break;
	}
	ifModify = false;
	return (str + "\n");
}





#ifdef TEST_HIDEFUNCHANDLE

int main()
{
	HideFuncHandle hf;
	std::string str = "test";
	int line = 1;
	bool ifModify = true;
	str = hf.ModifyForHideFunc(line, str, ifModify);
	std::cout << str<<std::endl;

	std::string str2 = "#ifdef";
	int line2 = 1;
	bool ifModify2 = true;
	str2 = hf.ModifyForHideFunc(line2, str2, ifModify2);
	std::cout << str2 << std::endl;

	return 0;
}

#endif
