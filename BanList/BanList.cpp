/*
* Dice! QQ Dice Robot for TRPG
* Copyright (C) 2018 w4123溯洄
*
* This program is free software: you can redistribute it and/or modify it under the terms
* of the GNU Affero General Public License as published by the Free Software Foundation,
* either version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License along with this
* program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <thread>
#include <chrono>
#include <mutex>

#include "APPINFO.h"
#include "CQEVE_ALL.h"
#include "CQTools.h"
#include "GlobalVar.h"
#include "DiceMsgSend.h"

using namespace std;
using namespace CQ;
const int MASTER = 448489320;

inline void init(string &msg)
{
	msg_decode(msg);
}
inline void init2(string &msg)
{
	for (int i = 0; i != msg.length(); i++)
	{
		if (msg[i] < 0)
		{
			if ((msg[i] & 0xff) == 0xa1 && (msg[i + 1] & 0xff) == 0xa1)
			{
				msg[i] = 0x20;
				msg.erase(msg.begin() + i + 1);
			}
			else if ((msg[i] & 0xff) == 0xa3 && (msg[i + 1] & 0xff) >= 0xa1 && (msg[i + 1] & 0xff) <= 0xfe)
			{
				msg[i] = msg[i + 1] - 0x80;
				msg.erase(msg.begin() + i + 1);
			}
			else
			{
				i++;
			}
		}
	}

	while (isspace(msg[0]))
		msg.erase(msg.begin());
	while (!msg.empty() && isspace(msg[msg.length() - 1]))
		msg.erase(msg.end() - 1);
	if (msg.substr(0, 2) == "。")
	{
		msg.erase(msg.begin());
		msg[0] = '.';
	}
	if (msg[0] == '!')
		msg[0] = '.';
}
std::string strip(std::string origin)
{
	bool flag = true;
	while (flag)
	{
		flag = false;
		if (origin[0]=='!' || origin[0] == '.')
		{
			origin.erase(origin.begin());
			flag = true;
		}
		else if (origin.substr(0,2) == "！"||origin.substr(0,2) == "。")
		{
			origin.erase(origin.begin());
			origin.erase(origin.begin());
			flag = true;
		}
	}
	return origin;
}
set<long long> BanedQQ;
struct SourceType {
	SourceType(long long a, int b, long long c) :QQ(a), Type(b), GrouporDiscussID(c) {};
	long long QQ = 0;
	int Type = 0;
	long long GrouporDiscussID = 0;
	bool operator<(SourceType b) const
	{
		return this->QQ < b.QQ;
	}
};
using PropType = map<string, int>;
string strFileLoc;
EVE_Enable(__eventEnable)
{
	//Wait until the thread terminates
	while (msgSendThreadRunning)
		Sleep(10);

	thread msgSendThread(SendMsg);
	msgSendThread.detach();
	strFileLoc = getAppDirectory();
	/*
	* 名称存储-创建与读取
	*/
	ifstream ifstreamBanedQQ(strFileLoc + "BanedQQ.RDconf");
	if (ifstreamBanedQQ)
	{
		long long QQNum;
		while (ifstreamBanedQQ >> QQNum)
		{
			BanedQQ.insert(QQNum);
		}
	}
	ifstreamBanedQQ.close();

	return 0;
}

EVE_PrivateMsg_EX(__eventPrivateMsg)
{
	if (eve.isSystem())return;
	init(eve.message);
	init2(eve.message);
	if (eve.message[0] != '.')
		return;
	int intMsgCnt = 1;
	while (isspace(eve.message[intMsgCnt]))
		intMsgCnt++;
	string strLowerMessage = eve.message;
	transform(strLowerMessage.begin(), strLowerMessage.end(), strLowerMessage.begin(), tolower);
	if (BanedQQ.count(eve.fromQQ))
	{
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "exit")
	{
	if (eve.fromQQ != MASTER) return;
		intMsgCnt += 4;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strgroup;
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strgroup += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		for (auto i : strgroup)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		const long long llgroup = stoll(strgroup);
		AddMsgToQueue("请勿将汪酱拉入非纯跑团用途的群中，如有需求请在群725116464中进行申请。", llgroup, false);
		if (setGroupLeave(llgroup) == 0)
			AddMsgToQueue("已从" + strgroup + "中退出", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "ban")
	{
	if (eve.fromQQ != MASTER) return;
		intMsgCnt += 3;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum;
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		for (auto i : strqqnum)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		const long long llqqnum = stoll(strqqnum);
		BanedQQ.insert(llqqnum);
		AddMsgToQueue("您因违规操作已被列入封禁名单！", llqqnum);
		AddMsgToQueue("已将" + strqqnum + "列入封禁名单！", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "unban")
	{

		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 5;
		while (isspace(strLowerMessage[intMsgCnt]))
		intMsgCnt++;
		string strqqnum;
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		for (auto i : strqqnum)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		const long long llqqnum = stoll(strqqnum);
		BanedQQ.erase(llqqnum);
		AddMsgToQueue(" 您已被移出封禁名单！", llqqnum);
		AddMsgToQueue("已将" + strqqnum + "移出封禁名单！", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "info")
	{
		intMsgCnt += 4;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		AddMsgToQueue(eve.message.substr(intMsgCnt), MASTER);
		eve.message_block();
		return;
	}
	return;
}
EVE_GroupMsg_EX(__eventGroupMsg)
{
	if (eve.isSystem() || eve.isAnonymous())return;
	else if (BanedQQ.count(eve.fromQQ))
	{
		eve.message_block();
		return;
	}
	else return;
}
EVE_DiscussMsg_EX(__eventDiscussMsg) 
{
	if (eve.isSystem())return;
	else if (BanedQQ.count(eve.fromQQ))
	{
		eve.message_block();
		return;
	}
	else return;
}
EVE_System_GroupMemberDecrease(__eventSystem_GroupMemberDecrease)
{
	if (beingOperateQQ == getLoginQQ())
	{
		BanedQQ.insert(fromQQ);
		AddMsgToQueue("您因违规操作已被列入封禁名单！", fromQQ);
		AddMsgToQueue("已将" + to_string(fromQQ) + "列入封禁名单！" + "原因：被踢出群" + to_string(fromGroup), MASTER);
		return 1;
	}
	return 0;
}
EVE_System_GroupMemberIncrease(__eventSystem_GroupMemberIncrease)
{
	if (beingOperateQQ == getLoginQQ())
	{
		AddMsgToQueue("欢迎使用汪酱掷骰机器人，反馈&交流群：725116464。输入.help获取使用说明。\n！！！请使用.dismiss指令使汪酱退群！！！", fromGroup, false);
	}
	return 1;
}
EVE_Request_AddFriend(__eventRequest_AddFriend)
{
	setFriendAddRequest(responseFlag, 1,"");
	return 1;
}
EVE_Request_AddGroup(__eventRequest_AddGroup)
{
	if (subType == 2)
	{
		if (BanedQQ.count(fromQQ))
		{
			return 1;
		}
		setGroupAddRequest(responseFlag, 2, 1, "");
		AddMsgToQueue(getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")邀请我加入群:" + getGroupList()[fromGroup] + "(" + to_string(fromGroup) + ")", MASTER);
		
	}
	return 0;
}
EVE_Disable(__eventDisable)
{
	Enabled = false;
	ofstream ofstreamBanedQQ(strFileLoc + "BanedQQ.RDconf", ios::out | ios::trunc);
	for (auto it = BanedQQ.begin(); it != BanedQQ.end(); ++it)
	{
		ofstreamBanedQQ << *it << std::endl;
	}
	ofstreamBanedQQ.close();

	return 0;
}
EVE_Exit(__eventExit)
{
	if (!Enabled)
		return 0;
	ofstream ofstreamBanedQQ(strFileLoc + "BanedQQ.RDconf", ios::out | ios::trunc);
	for (auto it = BanedQQ.begin(); it != BanedQQ.end(); ++it)
	{
		ofstreamBanedQQ << *it << std::endl;
	}
	ofstreamBanedQQ.close();

	return 0;
}
MUST_AppInfo_RETURN(CQAPPID);