/*
* Dice! QQ Dice Robot for TRPG
* Copyright (C) 2018 w4123���
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
#pragma once
#ifndef __DICEMSGSEND__
#define __DICEMSGSEND__
#include <string>

/*
 *  ����������Ϣ������Ϣ���Ͷ���
 *  Param:
 *  const std::string& msg ��Ϣ�����ַ���
 *  long long target_id Ŀ��ID(QQ,Ⱥ�Ż�������uin)
 *  bool is_private = true �Ƿ�Ϊ˽�� Ĭ��Ϊ��
 */
void AddMsgToQueue(const std::string& msg, long long target_id, bool is_private = true);

/*
 * ��Ϣ�����̺߳���
 * ע��: ���������߳��е��ô˺���, �˺��������ڳ�ʼ����Ϣ�����߳�
 */
void SendMsg();
#endif /*__DICEMSGSEND__*/

