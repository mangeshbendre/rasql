#ifndef _FACTORY_RA_H
#define _FACTORY_RA_H

#include "SymbolBase.h"
#include "UTFConverter.h"

#include <stdlib.h>
#include <list>
#include <map>
#include <sstream>

using namespace Astudillo;

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}


inline std::wstring trim_right(const std::wstring &source , const std::wstring& t = L" ")
{
	std::wstring str = source;
	return str.erase( str.find_last_not_of(t) + 1);
}

inline std::wstring trim_left( const std::wstring& source, const std::wstring& t = L" ")
{
	std::wstring str = source;
	return str.erase(0 , source.find_first_not_of(t) );
}

inline std::wstring trim(const std::wstring& source, const std::wstring& t = L" ")
{
	std::wstring str = source;
	return trim_left( trim_right( str , t) , t );
}

enum OptOperations {
	OptPushConditionInside,
	OptPushColumnsInside,
	OptPushRenameInside,
	OptPushRenameColInside
};

enum OptStatus {
	OptDone, OptNotDone, OptDontDo
};

typedef struct operation {
	OptOperations operation_code;
	OptStatus operationStatus;

	//Operation data
	wstring operationData;
	list<wstring> optcolumnlist;	// Operation Column List

} operation_t;

struct MyExecuteData: public ExecuteData {
	list<operation_t *> optlist;

	wstring relationName;
	list<wstring> columnlist; //Column List returned by the inside operation
	bool plainRelation; 	  // Plain relation, no select statement
	bool joinOperation;       // Binary operation

	MyExecuteData()
	{
		plainRelation=false;
		joinOperation=false;

	}

};


/* Utility functions */
class RelationAttributes{
public:
	static map<wstring, list<wstring> > relationMap; //Stores the realtion details
	static bool haveSchema;
	static list<wstring> emptyList; // Empty list

	static int addRelationAttributes(wstring relation, list<wstring> attributes)
	{
		relationMap[relation] = attributes;
		haveSchema=true;
		return 0;
	}

	static int clearRelationAttributes()
	{
		relationMap.clear();
		haveSchema=false;
		return 0;
	}

	static bool isSubset(list <wstring> sub_list, list<wstring> super_list){
	       list<wstring> :: iterator it_sub;
	       list<wstring> :: iterator it_super;

	       int size_sub = (int) sub_list.size();
	       int cnt_equal = 0;

	       for(it_sub=sub_list.begin(); it_sub != sub_list.end(); it_sub++){
	         for(it_super=super_list.begin(); it_super != super_list.end(); it_super++){
	              if((*it_sub) == (*it_super)){
	                   cnt_equal++;
	                   break;
	               }
	            }
	         }

	        if(cnt_equal == size_sub)
	             return true;
	        else
	        	return false;
	}

	static list<wstring> mergeColumn_list(list <wstring> list1, list <wstring> list2,
			wstring relation1, wstring relation2,
			int operation)
	{
		// 1 - Natural Join
		// 2- other joins
		list<wstring> out_list;

		if (list1.size()==0 || list2.size()==0)
			return RelationAttributes::emptyList;

		list<wstring>::iterator list_iter1;
		list<wstring>::iterator list_iter2;
		for (list_iter1 = list1.begin();
				list_iter1 != list1.end(); list_iter1++)
		{
			bool exists=false;
			for (list_iter2 = list2.begin();
					list_iter2 != list2.end(); list_iter2++)
			{
				if ((*list_iter1)==(*list_iter2))
				{
					exists = true;
					list_iter2 = list2.erase(list_iter2);
				}
			}
			if (exists && operation==2)
			{
				out_list.push_back(relation1 + L"." + *list_iter1);
				out_list.push_back(relation2 + L"." + *list_iter1);
			}
			else
			{
				out_list.push_back(*list_iter1);
			}
		}

		for (list_iter2 = list2.begin();
				list_iter2 != list2.end(); list_iter2++)
			out_list.push_back(*list_iter2);
		return out_list;
	}

	static wstring createColumnString(map<wstring, wstring> renameMap, list<wstring> column_list)
	{
		wstring ret;
		list<wstring>::iterator list_iter;
		for (list_iter = column_list.begin();
				list_iter != column_list.end(); list_iter++)
		{
			if (!ret.empty())
				ret += L", ";

			if (renameMap[*list_iter].empty() || renameMap[*list_iter] == *list_iter)
				ret += *list_iter;
			else
				ret += renameMap[*list_iter] + L" as " + *list_iter;
		}
		return ret;
	}

	/* Create a map to map the colums for rename */
	static map<wstring, wstring> createMapFromLists(map<wstring, wstring> retMap, list<wstring> key, list<wstring> value)
	{

		list<wstring>::iterator key_iter;
		list<wstring>::iterator value_iter;
		list<wstring> old_value_list;

		if (!retMap.empty())
		{
			for (value_iter = value.begin();
					value_iter != value.end(); value_iter++)
				old_value_list.push_back(retMap[*value_iter]);
		}
		else
		{
			old_value_list = value;
		}

		retMap.clear();
		value_iter = old_value_list.begin();
		for (key_iter = key.begin();
				key_iter != key.end(); key_iter++, value_iter++)
		{
			retMap[(*key_iter)]=(*value_iter);
		}
		return retMap;
	}
};


class Factory {
public:

	// TODO Update this after a new Grammer is Compiled
	enum eSymbol {
	       Symbol_EOF              =  0, // (EOF)
	       Symbol_Error            =  1, // (Error)
	       Symbol_Whitespace       =  2, // (Whitespace)
	       Symbol_CommentEnd       =  3, // (Comment End)
	       Symbol_CommentLine      =  4, // (Comment Line)
	       Symbol_CommentStart     =  5, // (Comment Start)
	       Symbol_Minus            =  6, // '-'
	       Symbol_ExclamEq         =  7, // '!='
	       Symbol_LParan           =  8, // '('
	       Symbol_RParan           =  9, // ')'
	       Symbol_Times            = 10, // '*'
	       Symbol_Comma            = 11, // ','
	       Symbol_Dot              = 12, // '.'
	       Symbol_Div              = 13, // '/'
	       Symbol_Semi             = 14, // ';'
	       Symbol_LBrace           = 15, // '{'
	       Symbol_RBrace           = 16, // '}'
	       Symbol_Plus             = 17, // '+'
	       Symbol_Lt               = 18, // '<'
	       Symbol_LtEq             = 19, // '<='
	       Symbol_LtGt             = 20, // '<>'
	       Symbol_Eq               = 21, // '='
	       Symbol_Gt               = 22, // '>'
	       Symbol_GtEq             = 23, // '>='
	       Symbol_AND              = 24, // AND
	       Symbol_BETWEEN          = 25, // BETWEEN
	       Symbol_Id               = 26, // Id
	       Symbol_IN               = 27, // IN
	       Symbol_IntegerLiteral   = 28, // IntegerLiteral
	       Symbol_INTERSECTION     = 29, // INTERSECTION
	       Symbol_IS               = 30, // IS
	       Symbol_JOIN             = 31, // JOIN
	       Symbol_LEFT_JOIN        = 32, // 'LEFT_JOIN'
	       Symbol_LIKE             = 33, // LIKE
	       Symbol_MINUS2           = 34, // MINUS
	       Symbol_NOT              = 35, // NOT
	       Symbol_NULL             = 36, // NULL
	       Symbol_OR               = 37, // OR
	       Symbol_OUTER_JOIN       = 38, // 'OUTER_JOIN'
	       Symbol_PRODUCT          = 39, // PRODUCT
	       Symbol_PROJECTION       = 40, // PROJECTION
	       Symbol_RealLiteral      = 41, // RealLiteral
	       Symbol_RENAME           = 42, // RENAME
	       Symbol_RIGHT_JOIN       = 43, // 'RIGHT_JOIN'
	       Symbol_SELECTION        = 44, // SELECTION
	       Symbol_StringLiteral    = 45, // StringLiteral
	       Symbol_UNION            = 46, // UNION
	       Symbol_AddExp           = 47, // <Add Exp>
	       Symbol_AndExp           = 48, // <And Exp>
	       Symbol_ColumnItem       = 49, // <Column Item>
	       Symbol_ColumnItemAlias  = 50, // <Column Item Alias>
	       Symbol_ColumnList       = 51, // <Column List>
	       Symbol_ColumnSource     = 52, // <Column Source>
	       Symbol_CreateTable      = 53, // <CreateTable>
	       Symbol_Difference       = 54, // <Difference>
	       Symbol_ExprList         = 55, // <Expr List>
	       Symbol_Expression       = 56, // <Expression>
	       Symbol_IdList           = 57, // <Id List>
	       Symbol_IdMember         = 58, // <Id Member>
	       Symbol_Intersection2    = 59, // <Intersection>
	       Symbol_Left_Join2       = 60, // <Left_Join>
	       Symbol_MultExp          = 61, // <Mult Exp>
	       Symbol_Natural_Join     = 62, // <Natural_Join>
	       Symbol_NegateExp        = 63, // <Negate Exp>
	       Symbol_NotExp           = 64, // <Not Exp>
	       Symbol_Outer_Join2      = 65, // <Outer_Join>
	       Symbol_PredExp          = 66, // <Pred Exp>
	       Symbol_Product2         = 67, // <Product>
	       Symbol_Projection2      = 68, // <Projection>
	       Symbol_Relation         = 69, // <Relation>
	       Symbol_Rename2          = 70, // <Rename>
	       Symbol_Rename_Attr      = 71, // <Rename_Attr>
	       Symbol_Right_Join2      = 72, // <Right_Join>
	       Symbol_Script           = 73, // <Script>
	       Symbol_Selection2       = 74, // <Selection>
	       Symbol_Theta_Join       = 75, // <Theta_Join>
	       Symbol_Theta_Left_Join  = 76, // <Theta_Left_Join>
	       Symbol_Theta_Outer_Join = 77, // <Theta_Outer_Join>
	       Symbol_Theta_Right_Join = 78, // <Theta_Right_Join>
	       Symbol_Tuple            = 79, // <Tuple>
	       Symbol_Union2           = 80, // <Union>
	       Symbol_Value            = 81  // <Value>
	};

	// Pass through any non handled Non-terminals and terminals
	class SymbolPassthrough: public SymbolBase {
		bool Execute(ExecuteValue* pResult, ExecuteData* pData) {
			int s32_Children = children.size();

			std::wstring::iterator end_pos;

			for (int i = 0; i < s32_Children; i++) {
				if (children[i]->type == TERMINAL) {
					Terminal* pIntValue;
					if (GetChildTerminal(i, &pIntValue, pData)) {
						pResult->String = trim(pResult->String);
						pResult->String += L" " + pIntValue->image;
					}
				} else {
					// NON_TERMINAL
					ExecuteValue Val1;
					if (((SymbolBase*) children[i])->Execute(&Val1, pData))
					{

						pResult->String = trim(pResult->String);
						pResult->String += L" " + Val1.String;
					}
					else
						return false;
				}
			}
			return true;
		}
	};


	// For column list return the list of columns
	class SymbolColumnItem: public SymbolBase {
		bool Execute(ExecuteValue* pResult, ExecuteData* pData) {

			Terminal* ColumnName;
			if (GetChildTerminal(0, &ColumnName, pData))
				pResult->String =  ColumnName->image;
			else
				return false;

			if (3==children.size())
			{
				if (GetChildTerminal(2, &ColumnName, pData))
					pResult->String += L"." + ColumnName->image;
				else
					return false;
			}
			((MyExecuteData *) pData)->columnlist.push_back(pResult->String);
			return true;
		}
	};


	class SymbolRelation: public SymbolBase {
		bool Execute(ExecuteValue* pResult, ExecuteData* pData) {
			int i=0;
			if (3==children.size()) i=1;
			if (children[i]->type == TERMINAL) {
				Terminal* pIntValue;
				if (GetChildTerminal(i, &pIntValue, pData))
				{
					pResult->String = pIntValue->image;
					((MyExecuteData *) pData)->plainRelation = true;
					((MyExecuteData *) pData)->relationName = pIntValue->image;
					if (RelationAttributes::haveSchema)
					{
						 map<wstring, list<wstring> >::iterator iter;
						 iter = RelationAttributes::relationMap.find(pResult->String);
						 if (iter != RelationAttributes::relationMap.end())
						 {
							 ((MyExecuteData *) pData)->columnlist = (*iter).second;
						 }
						 else
						 {
							 pData->SetError(this, "Invalid Table Name :" + wstrtostr(pResult->String));
							 return false;
						 }
					}
					else
					{
						((MyExecuteData *) pData)->columnlist = RelationAttributes::emptyList;
					}
				}
			} else {
				// NON_TERMINAL
				ExecuteValue Val1;
				if (((SymbolBase*) children[i])->Execute(&Val1, pData))
					pResult->String = Val1.String;
				else
					return false;
			}
			return true;
		}
	};

	class NestedRAOperations : public SymbolBase
	{
	protected:
		wstring Conditions;
		wstring JoinConditions;
		list<wstring> child_column_list;
		wstring Relations; /* will use later on for joins */
		wstring Relation_newName;
		bool distinct;
		operation_t *operation;
		bool operations_executed;
		map<wstring, wstring> rename_map;

		virtual bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{
			assert(0);
			return false;
		}


		bool ProcessCommands(list<operation_t *> optlist, MyExecuteData* pData)
		{
			Conditions.clear();
			distinct = false;

			// Start with matching map
			rename_map.clear();
			operations_executed=false;
			bool renamed=false;  // After rename skip conditions

			for (list<wstring>::iterator list_iter1 =
								child_column_list.begin(); list_iter1
								!= child_column_list.end(); list_iter1++)
				rename_map[*list_iter1]=*list_iter1;


			for (list<operation_t *>::reverse_iterator  list_iter =optlist.rbegin();
					list_iter != optlist.rend(); ++list_iter) {

				if ((*list_iter)->operationStatus == OptNotDone)
				{
					if (child_column_list.size() &&
							!RelationAttributes::isSubset((*list_iter)->optcolumnlist, child_column_list) &&
							(*list_iter)->operation_code <= OptPushColumnsInside)
					{
						pData->SetError(this, "Column not returned from inside");
						//////////////////////////
/*
						printf("\n subset [%d] cnt1 [%d] [%d}\n",
								RelationAttributes::isSubset((*list_iter)->optcolumnlist, child_column_list),
								child_column_list.size(), (*list_iter)->optcolumnlist.size()

						);


						for (list<wstring>::iterator list_iter1 =
							(*list_iter)->optcolumnlist.begin(); list_iter1
											!= (*list_iter)->optcolumnlist.end(); list_iter1++)
							printf("Opt list [%s]\n" , wstrtostr(*list_iter1).c_str());


						for (list<wstring>::iterator list_iter1 =
							child_column_list.begin(); list_iter1
											!= child_column_list.end(); list_iter1++)
						printf(" child list [%s]\n" , wstrtostr(*list_iter1).c_str());

*/

						///////////////////////////////


						return false;
					}

					switch ((*list_iter)->operation_code) {
						case OptPushConditionInside:
							// Work around
							if (renamed)
								return true;

							(*list_iter)->operationStatus=OptDone;

							if (Conditions.empty())
								Conditions = (*list_iter)->operationData;
							else
								Conditions = L"(" + Conditions + L") AND (" + (*list_iter)->operationData + L")";
							break;

						case OptPushColumnsInside:
							(*list_iter)->operationStatus=OptDone;
							distinct = true;
							child_column_list = (*list_iter)->optcolumnlist;
							break;

						case OptPushRenameInside:
							(*list_iter)->operationStatus=OptDone;
							Relation_newName =  (*list_iter)->operationData;
							renamed=true;
							break;

						case OptPushRenameColInside:
							(*list_iter)->operationStatus=OptDone;
							// (*list_iter)->optcolumnlist is new column list
							// child_column_list is the old column list

							// Make sure the rename column list and child column list have same count.
							if ((*list_iter)->optcolumnlist.size() != child_column_list.size())
							{
								pData->SetError(this, "Rename Column count does not match no of Columns returned");
								return false;
							}
							// Handles Multiple Renames
							rename_map =  RelationAttributes::createMapFromLists(rename_map, (*list_iter)->optcolumnlist, child_column_list);
							child_column_list = (*list_iter)->optcolumnlist;
							renamed=true;
							break;
					}
					operations_executed=true;
				}
			}
			return true;
		}

		bool GenerateSQL(ExecuteValue* pResult, MyExecuteData* pData)
		{
			if (!operations_executed && JoinConditions.empty())
			{
				pResult->String=Relations;
				return true;
			}

			if (!pData->plainRelation && JoinConditions.empty())
				Relations = L"(" + Relations + L")";

			wstring Columns = RelationAttributes::createColumnString(rename_map, child_column_list);

			if (RelationAttributes::haveSchema && Columns.empty())
			{
				pData->SetError(this, "Rename requires column information");
				return false;
			}
			else if (Columns.empty())
				Columns = L"*";
			else if (distinct)
				Columns = L"DISTINCT " + Columns;

			pResult->String = L"SELECT " + Columns + L" FROM " + Relations;

			if (!JoinConditions.empty())
				pResult->String += L" ON " + JoinConditions;

			if (!Conditions.empty())
				pResult->String += L" WHERE " + Conditions;

			if (!Relation_newName.empty())
			{
				pResult->String = L"(" + pResult->String +L") " + Relation_newName;
				pData->relationName = Relation_newName;
				pData->plainRelation = true;
			}
			else
			{
				// Since SQL is generated at this stage pass plain relation to false
				pData->plainRelation = false;
			}
			return true;
		}

		virtual bool disableOperationIfRequired(MyExecuteData * pData)
		{
			return true;
		}

		virtual bool enableOperationIfRequired(MyExecuteData * pData)
		{
			return true;
		}

		bool Execute(ExecuteValue* pResult, ExecuteData* pData)
		{
			operation = NULL;
			disableOperationIfRequired((MyExecuteData *) pData);

			if (!parseSymbol(pResult, (MyExecuteData *) pData))
				return false;

			enableOperationIfRequired( (MyExecuteData *) pData);

			if (!ProcessCommands(((MyExecuteData *) pData)->optlist, (MyExecuteData *) pData))
				return false;

			if (!GenerateSQL(pResult, (MyExecuteData *) pData))
				return false;

			// Finaly the expression will return child column list
			((MyExecuteData *) pData)->columnlist=child_column_list;
			return true;
		}
	};

	class SymbolSelection : public NestedRAOperations
	{
	public:
		bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{
			ExecuteValue Expression, Relation;
			pData->columnlist.clear();
			if (!ExecuteChild(2, &Expression, pData))
				return false;

			operation = new (operation_t);
			operation->operation_code=OptPushConditionInside;
			operation->operationData=Expression.String;
			operation->operationStatus=OptNotDone;
			operation->optcolumnlist=pData->columnlist;
			pData->optlist.push_back(operation);

			if (!ExecuteChild(5, &Relation, pData))
				return false;
			child_column_list=pData->columnlist;

			// Relation Name does not change
			Relations = Relation.String;
			return true;
		}

	};


	class SymbolProjection: public NestedRAOperations
	{
	public:
		bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{
			ExecuteValue Columns, Relation;
			pData->columnlist.clear();
			if (!ExecuteChild(2, &Columns, pData))
				return false;

			operation = new (operation_t);
			operation->operation_code=OptPushColumnsInside;
			operation->operationData=Columns.String;
			operation->operationStatus=OptNotDone;
			operation->optcolumnlist=pData->columnlist;
			pData->optlist.push_back(operation);

			if (!ExecuteChild(5, &Relation, pData))
				return false;
			child_column_list=pData->columnlist;

			Relations = Relation.String;
			return true;
		}
	};

	class JoinRAOperations : public NestedRAOperations
	{
		list<operation_t *> optlist_last;
		list<operation_t *> empty_list;
	protected:
		bool disableOperationIfRequired(MyExecuteData * pData)
		{
			optlist_last = pData->optlist;
			pData->optlist = empty_list;
			return true;
		}

		bool enableOperationIfRequired(MyExecuteData * pData)
		{
			pData->optlist = optlist_last;
			return true;
		}

		virtual wstring getJoinOperator()
		{
			return L" JOIN ";
		}

		bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{

			ExecuteValue Relation1, Relation2;
			list<wstring> child_column_list1;
			list<wstring> child_column_list2;
			wstring relation1_name;
			wstring relation2_name;

			pData->columnlist.clear();
			if (!ExecuteChild(0, &Relation1, pData))
				return false;
			child_column_list1 = pData->columnlist;
			relation1_name = pData->relationName;

			if (pData->plainRelation)
				Relations = Relation1.String + getJoinOperator();
			else
				Relations = L"(" + Relation1.String + L")" + getJoinOperator();

			pData->columnlist.clear();
			if (!ExecuteChild(2, &Relation2, pData))
				return false;
			child_column_list2 = pData->columnlist;
			relation2_name = pData->relationName;

			if (pData->plainRelation)
				Relations += Relation2.String;
			else
				Relations += L"(" + Relation2.String + L")";

			int opt=2;
			if (getJoinOperator().find(L"NATURAL"))
				opt=0;

			child_column_list = RelationAttributes::mergeColumn_list(child_column_list1, child_column_list2,
					relation1_name, relation2_name,
					opt);

			pData->plainRelation=true;
			pData->joinOperation=true;
			return true;
		}
	};


	class SymbolProduct : public JoinRAOperations
	{
	/* Empty Class */
	};

	class SymbolLeftJoin : public JoinRAOperations
	{
		wstring getJoinOperator()
		{
			return L" NATURAL LEFT JOIN ";
		}
	};

	class SymbolRightJoin : public JoinRAOperations
	{
		wstring getJoinOperator()
		{
			return L" NATURAL RIGHT JOIN ";
		}
	};

	class SymbolOuterJoin : public JoinRAOperations
	{
		wstring getJoinOperator()
		{
			return L" NATURAL OUTER JOIN ";
		}
	};


	class SymbolNaturalJoin : public JoinRAOperations
	{
		wstring getJoinOperator()
		{
			return L" NATURAL JOIN ";
		}
	};


	class SymbolThetaJoin : public JoinRAOperations
	{
	public:
		bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{
			ExecuteValue Relation1, Relation2, Condition1;
			list<wstring> child_column_list1;
			list<wstring> child_column_list2;
			wstring relation1_name;
			wstring relation2_name;

			pData->columnlist.clear();
			if (!ExecuteChild(0, &Relation1, pData))
					return false;
			child_column_list1 = pData->columnlist;
			relation1_name = pData->relationName;


			if (pData->plainRelation)
				Relations = Relation1.String + getJoinOperator();
			else
				Relations = L"(" + Relation1.String + L")" + getJoinOperator();

			pData->columnlist.clear();
			if (!ExecuteChild(5, &Relation2, pData))
					return false;
			child_column_list2 = pData->columnlist;
			relation2_name = pData->relationName;

			if (pData->plainRelation)
				Relations += Relation2.String;
			else
				Relations += L"(" + Relation2.String + L")";

			pData->columnlist.clear();
			if (!ExecuteChild(3, &Condition1, pData))
					return false;

			JoinConditions = Condition1.String;
			child_column_list = RelationAttributes::mergeColumn_list(child_column_list1, child_column_list2,
					relation1_name, relation2_name, 2);

			pData->plainRelation=false;
			pData->joinOperation=true;
			return true;
		}
	};

	class SymbolThetaLeftJoin : public SymbolThetaJoin
	{
		wstring getJoinOperator()
		{
			return L" LEFT JOIN ";
		}
	};

	class SymbolThetaRightJoin : public SymbolThetaJoin
	{
		wstring getJoinOperator()
		{
			return L" RIGHT JOIN ";
		}
	};

	class SymbolThetaOuterJoin : public SymbolThetaJoin
	{
		wstring getJoinOperator()
		{
			return L" OUTER JOIN ";
		}
	};


	class SymbolUnion : public JoinRAOperations
	{
		virtual wstring getJoinOperator()
		{
			return L" UNION ";
		}

		bool Execute(ExecuteValue* pResult, ExecuteData* pData)
		{
			ExecuteValue Relation1, Relation2;
			disableOperationIfRequired((MyExecuteData *)pData);
			if (!ExecuteChild(0, &Relation1, pData))
				return false;

			if (((MyExecuteData *)pData)->plainRelation)
				pResult->String=L"SELECT * FROM " + Relation1.String + getJoinOperator();
			else
				pResult->String=Relation1.String + getJoinOperator();

			if (!ExecuteChild(2, &Relation2, pData))
				return false;

			if (((MyExecuteData *)pData)->plainRelation)
				pResult->String+=L"SELECT * FROM " + Relation2.String;
			else
				pResult->String+=Relation2.String;

			enableOperationIfRequired((MyExecuteData *)pData);
			((MyExecuteData *)pData)->plainRelation = false;
			return true;
		}
	};

	class SymbolIntersect : public SymbolUnion
	{
		virtual wstring getJoinOperator()
		{
			return L" INTERSECT ";
		}
	};


	class SymbolDifference : public SymbolUnion
	{
		virtual wstring getJoinOperator()
		{
			return L" EXCEPT ";
		}
	};


	class SymbolRename : public NestedRAOperations
	{
	public:
		bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{
			Terminal* new_Relation1;
			if (!GetChildTerminal(2, &new_Relation1, pData))
				return false;

			operation = new (operation_t);
			operation->operation_code=OptPushRenameInside;
			operation->operationData=new_Relation1->image;
			operation->operationStatus=OptNotDone;
			pData->optlist.push_back(operation);

			Relation_newName=new_Relation1->image;

			ExecuteValue Relation1;
			if (!ExecuteChild(5, &Relation1, pData))
				return false;
			child_column_list=pData->columnlist;
			pData->relationName = Relation_newName;

			Relations = Relation1.String;

			return true;
		}
	};


	class SymbolRenameAttr : public NestedRAOperations
	{
		bool parseSymbol(ExecuteValue* pResult, MyExecuteData* pData)
		{
			Terminal* new_Relation1;
			pData->columnlist.clear();
			if (!GetChildTerminal(2, &new_Relation1, pData))
				return false;
			Relation_newName=new_Relation1->image;

			operation = new (operation_t);
			operation->operation_code=OptPushRenameInside;
			operation->operationData=new_Relation1->image;
			operation->operationStatus=OptNotDone;

			pData->optlist.push_back(operation);

			ExecuteValue NewColumns;

			pData->columnlist.clear();
			if (!ExecuteChild(4, &NewColumns, pData))
				return false;

			operation = new (operation_t);
			operation->operation_code=OptPushRenameColInside;
			operation->optcolumnlist=((MyExecuteData *) pData)->columnlist;
			operation->operationStatus=OptNotDone;

			pData->optlist.push_back(operation);

			pData->columnlist.clear();
			ExecuteValue Relation1;
			if (!ExecuteChild(8, &Relation1, pData))
				return false;
			child_column_list=pData->columnlist;
			pData->relationName = Relation_newName;

			Relations = Relation1.String;

			return true;
		}
	};

	class SymbolScript : public SymbolBase
	{
		bool Execute(ExecuteValue* pResult, ExecuteData* pData)
		{
			ExecuteValue Relation1, Relation2;
			if (((SymbolBase*) children[0])->Execute(&Relation1, pData))
			{
				if (((MyExecuteData *) pData)->plainRelation)
					pResult->String = L"SELECT * FROM " + Relation1.String;
				else
					pResult->String = Relation1.String;
			}
			else
			{
				return false;
			}

			if (3==children.size())
			{
				if (((SymbolBase*) children[2])->Execute(&Relation2, pData))
				{
					pResult->String += L";\n";
					if (((MyExecuteData *) pData)->plainRelation)
						pResult->String += L"SELECT * FROM (" + Relation2.String +L")";
					else
						pResult->String += Relation2.String;
				}
				else
				{
					return false;
				}
			}
			return true;
		}

	};

	class SymbolCreateTable : public SymbolBase
	{
		bool Execute(ExecuteValue* pResult, ExecuteData* pData)
		{
			Terminal* NewTable;
			if (GetChildTerminal(0, &NewTable, pData))
				pResult->String = L"CREATE TABLE " + NewTable->image + L" AS ";
			else
				return false;

			ExecuteValue Relation;
			if (((SymbolBase*) children[2])->Execute(&Relation, pData))
			{
				if (((MyExecuteData *) pData)->plainRelation)
					pResult->String += L"SELECT * FROM " + Relation.String;
				else
					pResult->String += Relation.String;
			}
			else
			{
				return false;
			}
			((MyExecuteData *)pData)->plainRelation = false;

			return true;

		}
	};
		// This is the factory that creates a class derived from NonTerminal according to the SymbolIndex
	static NonTerminal* CreateNewClass(int SymbolIndex)
	{
		switch(SymbolIndex)
		{
			case Symbol_Script:
				return new SymbolScript();

			case Symbol_CreateTable:
				return new SymbolCreateTable();

			case Symbol_Projection2:
				return new SymbolProjection();

			case Symbol_Selection2:
				return new SymbolSelection();

			case Symbol_Product2:
				return new SymbolProduct();

			case Symbol_Union2:
				return new SymbolUnion();

			case Symbol_Intersection2:
				return new SymbolIntersect();

			case Symbol_Difference:
				return new SymbolDifference();

			case Symbol_Left_Join2:
				return new SymbolLeftJoin();

			case Symbol_Right_Join2:
				return new SymbolRightJoin();

			case Symbol_Natural_Join:
				return new SymbolNaturalJoin();

			case Symbol_Outer_Join2:
				return new SymbolOuterJoin();

			case Symbol_Rename2:
				return new SymbolRename();

			case Symbol_Rename_Attr:
				return new SymbolRenameAttr();

			case Symbol_Relation:
				return new SymbolRelation();

			case Symbol_Theta_Join:
				return new SymbolThetaJoin();

			case Symbol_Theta_Left_Join:
				return new SymbolThetaLeftJoin();

			case Symbol_Theta_Right_Join:
				return new SymbolThetaRightJoin();

			case Symbol_Theta_Outer_Join:
				return new SymbolThetaOuterJoin();

			case Symbol_ColumnItem:
				return new SymbolColumnItem();

			default:
				return new SymbolPassthrough();

		}
	}
};

#endif

