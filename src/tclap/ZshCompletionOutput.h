// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/****************************************************************************** 
 * 
 *  file:  ZshCompletionOutput.h
 * 
 *  Copyright (c) 2006, Oliver Kiddle
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *  
 *****************************************************************************/ 

#ifndef TCLAP_ZSHCOMPLETIONOUTPUT_H
#define TCLAP_ZSHCOMPLETIONOUTPUT_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <map>

#include <tclap/CmdLineInterface.h>
#include <tclap/CmdLineOutput.h>
#include <tclap/XorHandler.h>
#include <tclap/Arg.h>

namespace TCLAP {

/**
 * A class that generates a Zsh completion function as output from the usage()
 * method for the given CmdLine and its Args.
 */
class ZshCompletionOutput : public CmdLineOutput
{

	public:

		ZshCompletionOutput();

		/**
		 * Prints the usage to stdout.  Can be overridden to 
		 * produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for. 
		 */
		virtual void usage(CmdLineInterface& c);

		/**
		 * Prints the version to stdout. Can be overridden 
		 * to produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for. 
		 */
		virtual void version(CmdLineInterface& c);

		/**
		 * Prints (to stderr) an error message, short usage 
		 * Can be overridden to produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for. 
		 * \param e - The ArgException that caused the failure. 
		 */
		virtual void failure(CmdLineInterface& c,
						     ArgException& e );

	protected:

		void basename( std::wstring& s );
		void quoteSpecialChars( std::wstring& s );

		std::wstring getMutexList( CmdLineInterface& _cmd, Arg* a );
		void printOption( Arg* it, std::wstring mutex );
		void printArg( Arg* it );

		std::map<std::wstring, std::wstring> common;
		char theDelimiter;
};

ZshCompletionOutput::ZshCompletionOutput()
: common(std::map<std::wstring, std::wstring>()),
  theDelimiter('=')
{
	common[L"host"] = L"_hosts";
	common[L"hostname"] = L"_hosts";
	common[L"file"] = L"_files";
	common[L"filename"] = L"_files";
	common[L"user"] = L"_users";
	common[L"username"] = L"_users";
	common[L"directory"] = L"_directories";
	common[L"path"] = L"_directories";
	common[L"url"] = L"_urls";
}

inline void ZshCompletionOutput::version(CmdLineInterface& _cmd)
{
	std::wcout << _cmd.getVersion() << std::endl;
}

inline void ZshCompletionOutput::usage(CmdLineInterface& _cmd )
{
	std::list<Arg*> argList = _cmd.getArgList();
	std::wstring progName = _cmd.getProgramName();
	std::wstring xversion = _cmd.getVersion();
	theDelimiter = _cmd.getDelimiter();
	basename(progName);

	std::wcout << "#compdef " << progName << std::endl << std::endl <<
		"# " << progName << " version " << _cmd.getVersion() << std::endl << std::endl <<
		"_arguments -s -S";

	for (ArgListIterator it = argList.begin(); it != argList.end(); it++)
	{
		if ( (*it)->shortID().at(0) == L'<' )
			printArg((*it));
		else if ( (*it)->getFlag() != L"-" )
			printOption((*it), getMutexList(_cmd, *it));
	}

	std::wcout << std::endl;
}

inline void ZshCompletionOutput::failure( CmdLineInterface& _cmd,
				                ArgException& e )
{
	static_cast<void>(_cmd); // unused
	std::wcout << e.what() << std::endl;
}

inline void ZshCompletionOutput::quoteSpecialChars( std::wstring& s )
{
	size_t idx = s.find_last_of(L':');
	while ( idx != std::wstring::npos )
	{
		s.insert(idx, 1, L'\\');
		idx = s.find_last_of(L':', idx);
	}
	idx = s.find_last_of(L'\'');
	while ( idx != std::wstring::npos )
	{
		s.insert(idx, L"'\\'");
		if (idx == 0)
			idx = std::wstring::npos;
		else
			idx = s.find_last_of(L'\'', --idx);
	}
}

inline void ZshCompletionOutput::basename( std::wstring& s )
{
	size_t p = s.find_last_of(L'/');
	if ( p != std::wstring::npos )
	{
		s.erase(0, p + 1);
	}
}

inline void ZshCompletionOutput::printArg(Arg* a)
{
	static int count = 1;

	std::wcout << L" \\" << std::endl << L"  '";
	if ( a->acceptsMultipleValues() )
		std::wcout << L'*';
	else
		std::wcout << count++;
	std::wcout << L':';
	if ( !a->isRequired() )
		std::wcout << L':';

	std::wcout << a->getName() << L':';
	std::map<std::wstring, std::wstring>::iterator compArg = common.find(a->getName());
	if ( compArg != common.end() )
	{
		std::wcout << compArg->second;
	}
	else
	{
		std::wcout << L"_guard \"^-*\" " << a->getName();
	}
	std::wcout << L'\'';
}

inline void ZshCompletionOutput::printOption(Arg* a, std::wstring mutex)
{
	std::wstring flag = a->flagStartChar() + a->getFlag();
	std::wstring name = a->nameStartString() + a->getName();
	std::wstring desc = a->getDescription();

	// remove full stop and capitalisation from description as
	// this is the convention for zsh function
	if (!desc.compare(0, 12, L"(required)  "))
	{
		desc.erase(0, 12);
	}
	if (!desc.compare(0, 15, L"(OR required)  "))
	{
		desc.erase(0, 15);
	}
	size_t len = desc.length();
	if (len && desc.at(--len) == L'.')
	{
		desc.erase(len);
	}
	if (len)
	{
		desc.replace(0, 1, 1, tolower(desc.at(0)));
	}

	std::wcout << L" \\" << std::endl << L"  '" << mutex;

	if ( a->getFlag().empty() )
	{
		std::wcout << name;
	}
	else
	{
		std::wcout << L"'{" << flag << L',' << name << L"}'";
	}
	if ( theDelimiter == '=' && a->isValueRequired() )
		std::wcout << L"=-";
	quoteSpecialChars(desc);
	std::wcout << L'[' << desc << L']';

	if ( a->isValueRequired() )
	{
		std::wstring arg = a->shortID();
		arg.erase(0, arg.find_last_of(theDelimiter) + 1);
		if ( arg.at(arg.length()-1) == L']' )
			arg.erase(arg.length()-1);
		if ( arg.at(arg.length()-1) == L']' )
		{
			arg.erase(arg.length()-1);
		}
		if ( arg.at(0) == L'<' )
		{
			arg.erase(arg.length()-1);
			arg.erase(0, 1);
		}
		size_t p = arg.find(L'|');
		if ( p != std::wstring::npos )
		{
			do
			{
				arg.replace(p, 1, 1, L' ');
			}
			while ( (p = arg.find_first_of(L'|', p)) != std::wstring::npos );
			quoteSpecialChars(arg);
			std::wcout << L": :(" << arg << L')';
		}
		else
		{
			std::wcout << L':' << arg;
			std::map<std::wstring, std::wstring>::iterator compArg = common.find(arg);
			if ( compArg != common.end() )
			{
				std::wcout << L':' << compArg->second;
			}
		}
	}

	std::wcout << '\'';
}

inline std::wstring ZshCompletionOutput::getMutexList( CmdLineInterface& _cmd, Arg* a)
{
	XorHandler xorHandler = _cmd.getXorHandler();
	std::vector< std::vector<Arg*> > xorList = xorHandler.getXorList();
	
	if (a->getName() == L"help" || a->getName() == L"version")
	{
		return L"(-)";
	}

	std::wostringstream list;
	if ( a->acceptsMultipleValues() )
	{
		list << L'*';
	}

	for ( int i = 0; static_cast<unsigned int>(i) < xorList.size(); i++ )
	{
		for ( ArgVectorIterator it = xorList[i].begin();
			it != xorList[i].end();
			it++)
		if ( a == (*it) )
		{
			list << '(';
			for ( ArgVectorIterator iu = xorList[i].begin();
				iu != xorList[i].end();
				iu++ )
			{
				bool notCur = (*iu) != a;
				bool hasFlag = !(*iu)->getFlag().empty();
				if ( iu != xorList[i].begin() && (notCur || hasFlag) )
					list << ' ';
				if (hasFlag)
					list << (*iu)->flagStartChar() << (*iu)->getFlag() << L' ';
				if ( notCur || hasFlag )
					list << (*iu)->nameStartString() << (*iu)->getName();
			}
			list << L')';
			return list.str();
		}
	}
	
	// wasn't found in xor list
	if (!a->getFlag().empty()) {
		list << L"(" << a->flagStartChar() << a->getFlag() << L' ' <<
			a->nameStartString() << a->getName() << L')';
	}
	
	return list.str();
}

} //namespace TCLAP
#endif
