// B Tree
// ALGORITHM:search and recursive_search,insert and adjust the B Tree after insertion, 
//           delete and ajust the B Tree after deletion,
//			 output the B Tree by HASM(Hierachical Sequential Access Method)
// DATASTRUCT:struct Node and queue
// AUTHOR: Huang Waleking
// JULY 15th,2006
////INCLUDE//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <math.h>
////MACRO//////////////////////////////////////////////////////////////////////////

#define ORDER 5    //5阶B树
#define MEDIAN (int)(ceil(((double)ORDER/2))-1)

////GLOBAL//////////////////////////////////////////////////////////////////////////

using namespace std;

enum FunctionState{Success,Not_present,Failed};

////STURCT//////////////////////////////////////////////////////////////////////////
template <typename T>
struct Node
{
	int data_count;//当前结点中关键字的个数
	T data[ORDER];//关键字数组,B树每个结点的关键字数量是ceil(order/2)-1~order-1,这里多留了一个位置方便调整
	Node<T> *branch[ORDER+1];//结点指针,指向下层结点
};
////CLASS//////////////////////////////////////////////////////////////////////////

template <typename T>
class BTree
{
public:
	BTree(){root=NULL;};
	FunctionState ReadFile();
	//~BTree();//用HSAM的方法删除各个结点

	Node<T>* GetRoot(){return root;};
	Node<T>* GetParent(Node<T>* current);//寻找双亲结点

	void HSAM(Node<T>* current);//层次遍历 Hierachical Sequential Access Method

	FunctionState SearchData(T& target,Node<T>* &current,int &position);//寻找结点中的一个关键字
	FunctionState SearchTree(T target);//寻找在B树中的一个结点和位置,为插入做准备
	FunctionState RecursiveSearchTree(Node<T>* &current,T &target);//递归查找

	FunctionState PushKey(T target,Node<T>* &temp,int & position,int tag=0);//往一个结点中加入一个关键字
	FunctionState Insert(T target);//插入的“主函数”
	void InsertAdjust(Node<T>* current);//插入后的调整

	FunctionState Delete(T target);//删除的“主函数”
	void Combine(Node<T>* current,Node<T>* parent,int position);
	void MoveLeft(Node<T>* current,Node<T>*parent,int position);
	void MoveRight(Node<T>* current,Node<T>*parent,int position);
	void DeleteAdjust();//删除后的调整

private:
	Node<T> *root;//B树的根指针
};
////CLASS MEMBER FUNCTION//////////////////////////////////////////////////////////////////////////
template <typename T>
FunctionState BTree<T>::ReadFile()
{
	ifstream infile("测试文件.txt");
	if(!infile)
	{
		cout<<"文件不存在\n";
		return Failed;
	}
	else
	{
		int num;
		while(infile>>num)
		{
			Insert(num);
		}
		return Success;
	}
	
}

//template <typename T>
//BTree<T> :: ~BTree()
//{
//	Node<T> * temp;
//	queue<Node<T>* > pSequential;
//	pSequential.push(root);
//	while(!pSequential.empty())
//	{
//		temp=pSequential.front();
//		pSequential.pop();
//		for(int i=0;i<=temp->data_count;i++)
//		{
//			if(temp->branch[i]!=NULL)
//				pSequential.push(temp->branch[i]);
//		}
//		delete temp;
//		//除了每层最后一个结点，删除时会出错，不知道为何？
//		//答：这是由delete的特性决定的，只能和new搭配使用，否则会出错
//		//	这里的各个结点都是由new产生的。但是，由于new和Node<T>*current在一起，而current
//		//	不断改变，造成除了新new出来，还没有被改变过的每层最后一个结点以外其他的都被看作
//		//	原有的，或者和new对应的指针丢失了。因而不能delete
//		//  ？？？？？
//	}
//	root=NULL;
//}

template <typename T>
void BTree<T>::HSAM(Node<T>*current)
{
	if(root==NULL||root->data_count==0)
		cout<<"[ null ]\n";
	else
	{	queue<Node<T>* > pSequential;
		Node<T> *temp;
		Node<T> *temp_next;
		int level=1;//结点层次
		cout<<"按层次遍历\n"<<"第1层：\n";
		pSequential.push(current);
		while(!pSequential.empty())
		{
			temp=pSequential.front();
			pSequential.pop();
			if(!pSequential.empty())
				temp_next=pSequential.front();//取temp下个指针，为比较关键字大小并输入换行符做准备
			//输出当前的各个关键字
			for(int i=0;i<temp->data_count;i++)
			{
				if(i==0)cout<<"[ ";
				cout<<temp->data[i]<<"  ";
				if(i==temp->data_count-1)cout<<"]  ";
			}
			//为按层次输出而进行的格式调整
			if(pSequential.empty() && temp->branch[0]!=NULL)
				cout<<'\n'<<"第"<<++level<<"层:\n";//如果队列暂时为空说明一层结点的输出完毕
			if(!pSequential.empty())
				if(temp->data[temp->data_count-1]>temp_next->data[0])
					cout<<'\n'<<"第"<<++level<<"层:\n";//如果队列不为空，就要用另一个判断条件：后面的关键字比前面的关键字小，说明这两关键字分属不同的层次
			//将下一层结点入队列
			for(int i=0;i<=temp->data_count;i++)
			{
				if(temp->branch[i]!=NULL)
					pSequential.push(temp->branch[i]);
			}
		}
		cout<<"\n\n";
	}
}

template <typename T>
Node<T>* BTree<T>::GetParent(Node<T>* current)
{
	if(current!=root)//如果不是根结点，用类似于HASM的方法找到双亲结点
	{
		Node<T> *parent;
		queue<Node<T>* > pSequential;
		Node<T>* temp;
		pSequential.push(root);
		while(!pSequential.empty())
		{
			temp=pSequential.front();
			pSequential.pop();
			for(int i=0;i<=temp->data_count;i++)
			{
				if(temp->branch[i]==current)
					return temp;
				if(temp->branch[i]!=NULL)
					pSequential.push(temp->branch[i]);
			}
		}
	}
	return NULL;
}

template <typename T>
FunctionState BTree<T>::SearchData(T& target,Node<T>* &current,int& position)
{
	position=0;
	while(position<current->data_count && target>current->data[position])
		position++;
	if(position<current->data_count && target==current->data[position])
		return Success;
	else
		return Not_present;
}

template <typename T>
FunctionState BTree<T>::SearchTree(T target)
{
	Node<T>* temp=root;
	return RecursiveSearchTree(temp,target);
}

template <typename T>
FunctionState BTree<T>::RecursiveSearchTree(Node<T>* &current,T&target)
{
	int position;
	FunctionState result;
	if(current!=NULL)
	{	
		result=SearchData(target,current,position);
		if(result==Not_present)
		{
			if(current->branch[position]!=NULL)
			{
				current=current->branch[position];
				result=RecursiveSearchTree(current,target);
			}
			else
				result=Not_present;
		}
	}
	return result;
}
////Insert
template <typename T>
FunctionState BTree<T>::PushKey(T target,Node<T>* &temp,int & position,int tag)
{
	//先找到要插入结点的位置
	if(tag==0)//tag为0,默认的搜索方式，往树中插入；若tag=1,表示往当前结点插入
		RecursiveSearchTree(temp,target);//找到应该插入的结点的位置
	SearchData(target,temp,position);//找到在结点中应该插入的位置
	//移动关键字
	if(position==temp->data_count)
	{
		temp->data[position]=target;
		temp->branch[position+1]=NULL;
	}
	else
	{
		for(int i=temp->data_count;i>position;i--)
		{
			temp->data[i]=temp->data[i-1];
			temp->branch[i+1]=temp->branch[i];
		}
		temp->data[position]=target;
		temp->branch[position+1]=NULL;
	}
	temp->data_count++;
	return Success;
}

template <typename T>
FunctionState BTree<T>::Insert(T target)
{
	if(root==NULL)
	{
		root=new Node<T>;
		root->data[0]=target;
		root->data_count=1;
		root->branch[0]=NULL;
		root->branch[1]=NULL;
	}
	else
	{
		Node<T>* temp=root;
		int position;
		if(SearchTree(target)==Success)
		{
			cout<<"要插入的这个关键字已经在B树中存在\n";
			return Failed;
		}
		else
		{
			PushKey(target,temp,position);
			InsertAdjust(temp);//调整结点，要分裂结点的都分裂
			return Success;
		}
	}
}

template <typename T>
void BTree<T>::InsertAdjust(Node<T>* current)
{
	if(current->data_count==ORDER)
	{
		//case 1:current==root,即根结点需要分裂
		if(current==root)
		{
			Node<T>* parent=new Node<T>;
			Node<T>* left_brother=new Node<T>;
			//分裂结点成left_brother和current
			for(int i=0;i<ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->data[i]=current->data[(int)(ceil(((double)ORDER/2))+i)];
			}
			for(int i=0;i<=ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->branch[i]=current->branch[(int)(ceil(((double)ORDER/2))+i)];
			}
			left_brother->data_count=(int)(ORDER-ceil(((double)ORDER/2)));
			//将MEDIAN升到双亲结点中去
			parent->data_count=1;
			parent->data[0]=current->data[MEDIAN];
			parent->branch[0]=current;
			parent->branch[1]=left_brother;
			//将原来的结点中多余的部分删去
			for(int i=MEDIAN+1;i<=ORDER+1;i++)
			{
				current->branch[i]=NULL;
			}
			current->data_count=MEDIAN;
			//根结点已经分裂，产生了新的根结点
			root=parent;
			root->data_count=parent->data_count;
		}
		//case 2:current!=root,即根结点不用分裂
		else
		{
			int position;
			Node<T>* parent=GetParent(current);
			Node<T>* left_brother=new Node<T>;
			//分裂结点成left_brother和current
			for(int i=0;i<ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->data[i]=current->data[(int)(ceil(((double)ORDER/2))+i)];
			}
			for(int i=0;i<=ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->branch[i]=current->branch[(int)(ceil(((double)ORDER/2))+i)];
			}
			left_brother->data_count=(int)(ORDER-ceil(((double)ORDER/2)));
			//将MEDIAN升到双亲结点中去
			PushKey(current->data[MEDIAN],parent,position,1);
			//将原来的结点中多余的部分删去
			for(int i=MEDIAN+1;i<=ORDER+1;i++)
			{
				current->branch[i]=NULL;
			}
			current->data_count=MEDIAN;
			//将PARENT中的POSITION+1处指针改为LEFT_BROTHER
			parent->branch[position+1]=left_brother;
			//然后再检查，调整PARENT
			InsertAdjust(parent);
		}
	}
}

////delete
template <typename T>
FunctionState BTree<T>::Delete(T target)
{
	Node<T> *temp=root;
	int position;
	if(SearchTree(target)==Not_present)
	{
		cout<<"没有找到要删除的关键字\n";
		return Failed;
	}
	else
	{
		RecursiveSearchTree(temp,target);//先找到是哪个结点
		SearchData(target,temp,position);//再找到是在该结点中的什么位置
		//case1:如果是叶子结点
		if(temp->branch[0]==NULL)
		{
			for(int i=position;i<temp->data_count;i++)
			{
				temp->data[i]=temp->data[i+1];
			}
			temp->data_count--;
		}
		//case2:非叶子结点
		else
		{
			//找该结点下的一个叶子结点，其中关键字最小的那个将覆盖要被删除的关键字
			Node<T>* offspring=temp->branch[position+1];
			T slipcover;
			while(offspring->branch[0]!=NULL)
			{
				offspring=offspring->branch[0];
			}
			slipcover=offspring->data[0];
			for(int i=0;i<offspring->data_count;i++)
			{
				offspring->data[i]=offspring->data[i+1];
			}
			offspring->data_count--;
			temp->data[position]=slipcover;
		}
		//调整B树
		DeleteAdjust();
	}
	return Success;
}

template <typename T>
void BTree<T>::DeleteAdjust()
{
	//先遍历B树，找到一个非根结点，它的关键字个数小于MEDIAN
	Node<T>* current;
	queue<Node<T>* > pSequential;
	pSequential.push(root);
	while(!pSequential.empty())
	{
		current=pSequential.front();
		pSequential.pop();
		for(int i=0;i<=current->data_count;i++)
		{
			if(current->branch[i]!=NULL)
				pSequential.push(current->branch[i]);
		}
		if(current!=root && current->data_count<MEDIAN)
			break;
	}
	if(current!=root&&current->data_count<MEDIAN)//这是调整B树的条件
	{
		Node<T>* parent=GetParent(current);
		int position;
		for(int i=0;i<=parent->data_count;i++)
		{
			if(parent->branch[i]==current)
			{
				position=i;
				break;
			}
		}
		//step1.move_left
		if(position!=parent->data_count && parent->branch[position+1]->data_count>MEDIAN)
		{
			Node<T> *right_brother=parent->branch[position+1];
			MoveLeft(current,parent,position);
		}
		//step2.move_right
		else if(position!=0 && parent->branch[position-1]->data_count>MEDIAN)
		{
			Node<T> *left_brother=parent->branch[position-1];
			MoveRight(current,parent,position);
		}
		//step3.combine
		else
		{
			Combine(current,parent,position);
		}
		//step4.再检查一下，需要调整则继续调整
		DeleteAdjust();
	}
}

template <typename T>
void BTree<T>::MoveLeft(Node<T>* current,Node<T>*parent,int position)
{
	Node<T>* right_brother=parent->branch[position+1];
	current->data[current->data_count]=parent->data[position];
	current->branch[current->data_count+1]=right_brother->branch[0];
	parent->data[position]=right_brother->data[0];
	for(int i=0;i<right_brother->data_count;i++)
	{
		right_brother->data[i]=right_brother->data[i+1];
		right_brother->branch[i]=right_brother->branch[i+1];
	}
	current->data_count++;
	right_brother->data_count--;
}

template <typename T>
void BTree<T>::MoveRight(Node<T>* current,Node<T>*parent,int position)
{
	Node<T>* left_brother=parent->branch[position-1];
	for(int i=current->data_count;i>0;i--)
		current->data[i]=current->data[i-1];
	for(int i=current->data_count+1;i>0;i--)
		current->branch[i]=current->branch[i-1];
	current->data[0]=parent->data[position-1];
	current->branch[0]=left_brother->branch[left_brother->data_count];
	parent->data[position-1]=left_brother->data[left_brother->data_count-1];
	left_brother->data_count--;
	current->data_count++;
}

template <typename T>
void BTree<T>::Combine(Node<T>* current,Node<T>* parent,int position)
{
	Node<T>* left_brother;
	Node<T>* right_brother;
	if(position!=0)
	{
		left_brother=parent->branch[position-1];//找左兄弟优先
		right_brother=current;
		//调整左兄弟结点
		left_brother->data[left_brother->data_count]=parent->data[position-1];
		for(int i=0;i<right_brother->data_count;i++)
		{
			left_brother->data[left_brother->data_count+i+1]=right_brother->data[i];
		}
		for(int i=0;i<right_brother->data_count+1;i++)
		{
			left_brother->branch[left_brother->data_count+i+1]=right_brother->branch[i];
		}
		left_brother->data_count=left_brother->data_count+right_brother->data_count+1;
		//调整右兄弟
		right_brother->data_count=0;
		//调整双亲结点
		for(int i=position-1;i<parent->data_count;i++)
			parent->data[i]=parent->data[i+1];
		for(int i=position;i<parent->data_count;i++)
			parent->branch[i]=parent->branch[i+1];
		parent->data_count--;
		if(parent==root && parent->data_count==0)
			root=left_brother;
	}
	else
	{
		right_brother=parent->branch[1];
		left_brother=current;
		//调整左兄弟
		left_brother->data[left_brother->data_count]=parent->data[position];
		for(int i=0;i<right_brother->data_count;i++)
		{
			left_brother->data[left_brother->data_count+i+1]=right_brother->data[i];
		}
		for(int i=0;i<right_brother->data_count+1;i++)
		{
			left_brother->branch[left_brother->data_count+i+1]=right_brother->branch[i];
		}
		left_brother->data_count=left_brother->data_count+right_brother->data_count+1;
		//调整右兄弟
		right_brother->data_count=0;
		//调整双亲结点
		for(int i=position;i<parent->data_count;i++)
			parent->data[i]=parent->data[i+1];
		for(int i=position+1;i<parent->data_count;i++)
			parent->branch[i]=parent->branch[i+1];
		parent->data_count--;
		if(parent==root && parent->data_count==0)
			root=left_brother;
	} 
}

////MAIN FUNCTION//////////////////////////////////////////////////////////////////////////
int main()
{
	BTree<int> tree;
	int choice;
	double key;
	cout<<"B树及插入，删除和查找\n\n"
		<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~B树~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
		 <<"1.选择从测试文件读入关键字建立B树\n"
		 <<"2.选择从键盘输入，向B树插入关键字\n"
		 <<"3.输出B树\n"
		 <<"4.删除关键字\n"
		 <<"5.查找关键字\n"
		 <<"6.退出\n"
		 <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
	while(cin>>choice)//当输入key为'a'时，影响到了这一句的成立//？？
	{
		switch(choice)
		{
		case 1:
			{
				cout<<"选择了1，从测试文件读入\n";
				tree.ReadFile();
			}break;
		case 2:
			{
				cout<<"选择了2，从键盘输入。请输入关键字：";
				if(cin>>key)
				{
					if((int)key==key)
					{tree.Insert((int)key);cout<<(int)key<<"已经插入树中\n";}
					else cout<<"输入有误";
				}
				else
					cout<<"输入有误\n";
			}break;
		case 3:
			{
				cout<<"选择了3\n";
				tree.HSAM(tree.GetRoot());
			}break;
		case 4:
			{
				cout<<"选择了4.请输入要删除的关键字：";
				if(cin>>key)
				{
					if((int)key==key)
					{
						if(tree.SearchTree((int)key)==Success)
						{
							tree.Delete((int)key);
							cout<<(int)key<<"已删除\n";
						}
						else
							cout<<(int)key<<"不在树中，无法删除\n";
					}
				}
				else
					cout<<"输入有误\n";				
			}break;
		case 5:
			{
				cout<<"选择了5。请输入要查找的关键字：";
				if(cin>>key)
				{
					if((int)key==key)
					{
						if(tree.SearchTree((int)key)==Success)
							cout<<(int)key<<"在这棵B树中\n";
						else
							cout<<(int)key<<"不在这棵B树中\n";
					}
				}
				else
					cout<<"输入有误\n";	
			}break;
		case 6:
			{
				return 0;
			}break;
		default :
			cout<<"输入的命令有误，请重新输入\n";break;
		}
		cout<<"\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~B树~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
		 <<"1.选择从测试文件读入关键字建立B树\n"
		 <<"2.选择从键盘输入，向B树插入关键字\n"
		 <<"3.输出B树\n"
		 <<"4.删除关键字\n"
		 <<"5.查找关键字\n"
		 <<"6.退出\n"
		 <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
	}
	return 0;
}