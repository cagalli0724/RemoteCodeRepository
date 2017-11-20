/////////////////////////////////////////////////////////////////////
//  Tokenizer.cpp - Reads words from a file                        //
//  ver 1.9                                                        //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include "dbTokenizer.h"

//----< constructor may be called with no argument >-----------
namespace XmlProcessing 
{
	void XmlToker::returnComments(bool doReturn)
	{
		doReturnComments = doReturn;
	}

	bool XmlToker::isFileEnd()
	{
		return (nextChar == -1);
	}

	int& XmlToker::lines()
	{
		return numLines;
	}

	int XmlToker::braceLevel()
	{
		return braceCount;
	}

	XmlToker::XmlToker(const std::string& src, bool isFile)
		: prevprevChar(0), prevChar(0), currChar(0), nextChar(0),
		doReturnComments(false), numLines(0), braceCount(0), pIn(0),
		_state(default_state)
	{
		if (src.length() > 0)
		{
			if (!attach(src, isFile))
			{
				std::string temp = std::string("can't open ") + src;
				throw std::exception(temp.c_str());
			}
		}
		scTok = "()[]{};.\n";
		if (_mode == xml)
		{
			scTok = "<>!" + scTok;
		}
	}
	//----< destructor >-------------------------------------------

	XmlToker::~XmlToker()
	{
		if (pIn)
		{
			pIn->clear();
			std::ifstream* pFs = dynamic_cast<std::ifstream*>(pIn);
			if (pFs)
			{
				pFs->close();
			}
			delete pIn;
		}
	}
	//----< set mode of tokenizing, e.g., code or xml >------------

	void XmlToker::setMode(XmlToker::mode md)
	{
		_mode = md;
		scTok = "()[]{};.\n";
		if (_mode == xml)
		{
			scTok = "<>!" + scTok;
		}
	}
	//----< set mode of tokenizing, e.g., code or xml >------------

	void XmlToker::setSingleCharTokens(std::string tokChars)
	{
		_mode = custom;
		scTok = tokChars;
	}
	//----< attach tokenizer to a source file or string >----------

	bool XmlToker::attach(const std::string& name, bool srcIsFile)
	{
		if (pIn && srcIsFile)
		{
			pIn->clear();
			std::ifstream* pFs = dynamic_cast<std::ifstream*>(pIn);
			if (pFs)
			{
				pFs->close();
			}
			delete pIn;
		}
		if (srcIsFile)
		{
			pIn = new std::ifstream(name.c_str());
		}
		else
		{
			pIn = new std::istringstream(name.c_str());
		}
		return pIn->good();
	}
	//----< peek function that works with multiple putbacks >------

	int XmlToker::peek()
	{
		if (putbacks.size() > 0)
		{
			return putbacks[putbacks.size() - 1];
		}
		else
		{
			return pIn->peek();
		}

	}
	//----< multiple putBack that won't break peek >---------------

	void XmlToker::putback(int ch)
	{
		putbacks.push_back(ch);
		nextChar = ch;
		currChar = prevChar;
		prevChar = prevprevChar;
	}
	//----< get consistent with peek and putback >-----------------

	int XmlToker::get()
	{
		if (putbacks.size() > 0)
		{
			char ch = putbacks.front();
			putbacks.pop_back();
			return ch;
		}
		return pIn->get();
	}
	//
	//----< extract character from attached stream >---------------

	bool XmlToker::getChar()
	{
		char oldNext = nextChar;
		prevprevChar = prevChar;
		prevChar = currChar;
		currChar = this->get();
		nextChar = this->peek();
		_ASSERT(currChar == oldNext || oldNext == 0);
		if (currChar == '\n')
		{
			++numLines;
		}
		if (currChar == '{' && _state == default_state)
		{
			++braceCount;
		}
		if (currChar == '}' && _state == default_state)
		{
			--braceCount;
		}
		return !pIn->eof();
	}
	//----< is this char a single char token? >--------------------

	bool XmlToker::isSingleCharTok(char ch)
	{
		if (scTok.find(ch) < scTok.length())
			return true;
		return false;
	}
	//----< remove contiguous white space except for newline >-----

	void XmlToker::stripWhiteSpace()
	{
		if (nextChar == '\n')
			return;
		while (isspace(nextChar) && nextChar != '\n')
		{
			getChar();
		}
	}
	//----< is this an indentifier character? >--------------------

	bool XmlToker::isIdentifierChar(char ch)
	{
		if (isalpha(ch) || ch == '_' || isdigit(ch))
			return true;
		return false;
	}
	//----< is this the end of a token? >--------------------------

	bool XmlToker::isTokEnd()
	{
		if (isspace(nextChar))
			return true;
		if (isSingleCharTok(nextChar) || isSingleCharTok(currChar))
			return true;
		if (isIdentifierChar(currChar) && !isIdentifierChar(nextChar))
			return true;
		if (!isIdentifierChar(currChar) && isIdentifierChar(nextChar))
			return true;
		if (isFileEnd())
			return true;
		return false;
	}
	//----< is this the beginning of a comment? >------------------

	bool XmlToker::isBeginComment()
	{
		if (prevChar != '\\' && currChar == '/' && nextChar == '*')
		{
			aCppComment = false;
			return true;
		}
		if (prevChar != '\\' && currChar == '/' && nextChar == '/')
		{
			aCppComment = true;
			return true;
		}
		return false;
	}
	//----< is this the end of a comment? >------------------------

	bool XmlToker::isEndComment()
	{
		if (aCppComment && currChar != '\\' && nextChar == '\n')
			return true;
		if (!aCppComment && prevChar != '\\' && currChar == '*' && nextChar == '/')
			return true;
		return false;
	}
	//----< return comment as a token >----------------------------

	std::string XmlToker::eatComment()
	{
		_state = comment_state;
		std::string tok(1, currChar);
		while (!isEndComment() && pIn->good())
		{
			getChar();
			tok.append(1, currChar);
		}

		if (!aCppComment)
		{
			getChar();
			tok.append(1, currChar);
		}
		_state = default_state;
		return tok;
	}
	//----< is this the beginning of a quote? >--------------------

	bool XmlToker::isBeginQuote()
	{
		if (prevChar != '\\' && currChar == '\'')
		{
			aSingleQuote = true;
			return true;
		}
		if (prevChar != '\\' && currChar == '\"')
		{
			aSingleQuote = false;
			return true;
		}
		return false;
	}
	//----< is this the end of quote? >----------------------------

	bool XmlToker::isEndQuote()
	{
		if (prevChar == '\\' || currChar != '\\')
		{
			if (aSingleQuote && nextChar == '\'')
				return true;
			if (!aSingleQuote && nextChar == '\"')
				return true;
		}
		return false;
	}
	//----< return single or double quote as token >---------------

	std::string XmlToker::eatQuote()
	{
		_state = quote_state;
		std::string tok(1, currChar);
		while (!isEndQuote())
		{
			if (!pIn->good())
				throw std::exception("missing end of quote");
			getChar();
			tok.append(1, currChar);
		}
		getChar();
		tok.append(1, currChar);
		_state = default_state;
		return tok;
	}
	//----< read token from attached file >------------------------

	std::string XmlToker::getTok()
	{
		std::string tok = "";
		stripWhiteSpace();
		if (isSingleCharTok(nextChar))
		{
			getChar();
			tok.append(1, currChar);
			return tok;
		}
		do
		{
			if (isFileEnd())
				return tok;

			getChar();
			if (isBeginComment())
			{
				if (tok.length() > 0)
				{
					this->putback(currChar);
					return tok;
				}
				tok = eatComment();
				if (doReturnComments)
					return tok;
				else
				{
					tok = "";
					continue;
				}
			}
			if (isBeginQuote())
			{
				if (tok.length() > 0)
				{
					this->putback(currChar);
					return tok;
				}
				tok = eatQuote();
				return tok;
			}
			if (!isspace(currChar))
				tok.append(1, currChar);
		} while (!isTokEnd() || tok.length() == 0);
		return tok;
	}
}

//----< test stub >--------------------------------------------

#ifdef TEST_TOKENIZER
using namespace XmlProcessing;
int main(int argc, char* argv[])
{
	std::cout << "\n  Testing Tokenizer class\n "
		<< std::string(25, '=') << std::endl;
	std::cout
		<< "\n  Note that comments and quotes are returned as single tokens\n\n";

	// collecting tokens from a string

	Toker t_fromStr("tokens from a string: -> int x; /* a comment */", false);
	std::string tok;
	do
	{
		tok = t_fromStr.getTok();
		std::cout << "  " << tok;
	} while (tok != "");
	std::cout << "\n\n";

	// collecting tokens from files, named on the command line

	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for (int i = 1; i<argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]), '-') << "\n";

		try
		{
			Toker t;
			t.setMode(Toker::xml);        // comment out to show tokenizing for code

			if (!t.attach(argv[i]))
			{
				std::cout << "\n    can't open file " << argv[i] << "\n\n";
				continue;
			}
			t.returnComments();  // remove this statement to discard comment tokens
			std::string temp;
			do
			{
				temp = t.getTok();
				std::cout << "  ln: " << t.lines() << ", br lev: "
					<< t.braceLevel() << ", tok size: " << std::setw(3) << temp.length() << " -- ";
				if (temp != "\n")
					std::cout << temp << std::endl;
				else
					std::cout << "newline\n";
			} while (temp != "");
		}
		catch (std::exception& ex)
		{
			std::cout << "  " << ex.what() << "\n\n";
		}
	}
}


#endif
