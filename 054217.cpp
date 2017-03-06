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

#define ORDER 5    //5��B��
#define MEDIAN (int)(ceil(((double)ORDER/2))-1)

////GLOBAL//////////////////////////////////////////////////////////////////////////

using namespace std;

enum FunctionState{Success,Not_present,Failed};

////STURCT//////////////////////////////////////////////////////////////////////////
template <typename T>
struct Node
{
	int data_count;//��ǰ����йؼ��ֵĸ���
	T data[ORDER];//�ؼ�������,B��ÿ�����Ĺؼ���������ceil(order/2)-1~order-1,���������һ��λ�÷������
	Node<T> *branch[ORDER+1];//���ָ��,ָ���²���
};
////CLASS//////////////////////////////////////////////////////////////////////////

template <typename T>
class BTree
{
public:
	BTree(){root=NULL;};
	FunctionState ReadFile();
	//~BTree();//��HSAM�ķ���ɾ���������

	Node<T>* GetRoot(){return root;};
	Node<T>* GetParent(Node<T>* current);//Ѱ��˫�׽��

	void HSAM(Node<T>* current);//��α��� Hierachical Sequential Access Method

	FunctionState SearchData(T& target,Node<T>* &current,int &position);//Ѱ�ҽ���е�һ���ؼ���
	FunctionState SearchTree(T target);//Ѱ����B���е�һ������λ��,Ϊ������׼��
	FunctionState RecursiveSearchTree(Node<T>* &current,T &target);//�ݹ����

	FunctionState PushKey(T target,Node<T>* &temp,int & position,int tag=0);//��һ������м���һ���ؼ���
	FunctionState Insert(T target);//����ġ���������
	void InsertAdjust(Node<T>* current);//�����ĵ���

	FunctionState Delete(T target);//ɾ���ġ���������
	void Combine(Node<T>* current,Node<T>* parent,int position);
	void MoveLeft(Node<T>* current,Node<T>*parent,int position);
	void MoveRight(Node<T>* current,Node<T>*parent,int position);
	void DeleteAdjust();//ɾ����ĵ���

private:
	Node<T> *root;//B���ĸ�ָ��
};
////CLASS MEMBER FUNCTION//////////////////////////////////////////////////////////////////////////
template <typename T>
FunctionState BTree<T>::ReadFile()
{
	ifstream infile("�����ļ�.txt");
	if(!infile)
	{
		cout<<"�ļ�������\n";
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
//		//����ÿ�����һ����㣬ɾ��ʱ�������֪��Ϊ�Σ�
//		//��������delete�����Ծ����ģ�ֻ�ܺ�new����ʹ�ã���������
//		//	����ĸ�����㶼����new�����ġ����ǣ�����new��Node<T>*current��һ�𣬶�current
//		//	���ϸı䣬��ɳ�����new��������û�б��ı����ÿ�����һ��������������Ķ�������
//		//	ԭ�еģ����ߺ�new��Ӧ��ָ�붪ʧ�ˡ��������delete
//		//  ����������
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
		int level=1;//�����
		cout<<"����α���\n"<<"��1�㣺\n";
		pSequential.push(current);
		while(!pSequential.empty())
		{
			temp=pSequential.front();
			pSequential.pop();
			if(!pSequential.empty())
				temp_next=pSequential.front();//ȡtemp�¸�ָ�룬Ϊ�ȽϹؼ��ִ�С�����뻻�з���׼��
			//�����ǰ�ĸ����ؼ���
			for(int i=0;i<temp->data_count;i++)
			{
				if(i==0)cout<<"[ ";
				cout<<temp->data[i]<<"  ";
				if(i==temp->data_count-1)cout<<"]  ";
			}
			//Ϊ�������������еĸ�ʽ����
			if(pSequential.empty() && temp->branch[0]!=NULL)
				cout<<'\n'<<"��"<<++level<<"��:\n";//���������ʱΪ��˵��һ�����������
			if(!pSequential.empty())
				if(temp->data[temp->data_count-1]>temp_next->data[0])
					cout<<'\n'<<"��"<<++level<<"��:\n";//������в�Ϊ�գ���Ҫ����һ���ж�����������Ĺؼ��ֱ�ǰ��Ĺؼ���С��˵�������ؼ��ַ�����ͬ�Ĳ��
			//����һ���������
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
	if(current!=root)//������Ǹ���㣬��������HASM�ķ����ҵ�˫�׽��
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
	//���ҵ�Ҫ�������λ��
	if(tag==0)//tagΪ0,Ĭ�ϵ�������ʽ�������в��룻��tag=1,��ʾ����ǰ������
		RecursiveSearchTree(temp,target);//�ҵ�Ӧ�ò���Ľ���λ��
	SearchData(target,temp,position);//�ҵ��ڽ����Ӧ�ò����λ��
	//�ƶ��ؼ���
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
			cout<<"Ҫ���������ؼ����Ѿ���B���д���\n";
			return Failed;
		}
		else
		{
			PushKey(target,temp,position);
			InsertAdjust(temp);//������㣬Ҫ���ѽ��Ķ�����
			return Success;
		}
	}
}

template <typename T>
void BTree<T>::InsertAdjust(Node<T>* current)
{
	if(current->data_count==ORDER)
	{
		//case 1:current==root,���������Ҫ����
		if(current==root)
		{
			Node<T>* parent=new Node<T>;
			Node<T>* left_brother=new Node<T>;
			//���ѽ���left_brother��current
			for(int i=0;i<ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->data[i]=current->data[(int)(ceil(((double)ORDER/2))+i)];
			}
			for(int i=0;i<=ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->branch[i]=current->branch[(int)(ceil(((double)ORDER/2))+i)];
			}
			left_brother->data_count=(int)(ORDER-ceil(((double)ORDER/2)));
			//��MEDIAN����˫�׽����ȥ
			parent->data_count=1;
			parent->data[0]=current->data[MEDIAN];
			parent->branch[0]=current;
			parent->branch[1]=left_brother;
			//��ԭ���Ľ���ж���Ĳ���ɾȥ
			for(int i=MEDIAN+1;i<=ORDER+1;i++)
			{
				current->branch[i]=NULL;
			}
			current->data_count=MEDIAN;
			//������Ѿ����ѣ��������µĸ����
			root=parent;
			root->data_count=parent->data_count;
		}
		//case 2:current!=root,������㲻�÷���
		else
		{
			int position;
			Node<T>* parent=GetParent(current);
			Node<T>* left_brother=new Node<T>;
			//���ѽ���left_brother��current
			for(int i=0;i<ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->data[i]=current->data[(int)(ceil(((double)ORDER/2))+i)];
			}
			for(int i=0;i<=ORDER-ceil(((double)ORDER/2));i++)
			{
				left_brother->branch[i]=current->branch[(int)(ceil(((double)ORDER/2))+i)];
			}
			left_brother->data_count=(int)(ORDER-ceil(((double)ORDER/2)));
			//��MEDIAN����˫�׽����ȥ
			PushKey(current->data[MEDIAN],parent,position,1);
			//��ԭ���Ľ���ж���Ĳ���ɾȥ
			for(int i=MEDIAN+1;i<=ORDER+1;i++)
			{
				current->branch[i]=NULL;
			}
			current->data_count=MEDIAN;
			//��PARENT�е�POSITION+1��ָ���ΪLEFT_BROTHER
			parent->branch[position+1]=left_brother;
			//Ȼ���ټ�飬����PARENT
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
		cout<<"û���ҵ�Ҫɾ���Ĺؼ���\n";
		return Failed;
	}
	else
	{
		RecursiveSearchTree(temp,target);//���ҵ����ĸ����
		SearchData(target,temp,position);//���ҵ����ڸý���е�ʲôλ��
		//case1:�����Ҷ�ӽ��
		if(temp->branch[0]==NULL)
		{
			for(int i=position;i<temp->data_count;i++)
			{
				temp->data[i]=temp->data[i+1];
			}
			temp->data_count--;
		}
		//case2:��Ҷ�ӽ��
		else
		{
			//�Ҹý���µ�һ��Ҷ�ӽ�㣬���йؼ�����С���Ǹ�������Ҫ��ɾ���Ĺؼ���
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
		//����B��
		DeleteAdjust();
	}
	return Success;
}

template <typename T>
void BTree<T>::DeleteAdjust()
{
	//�ȱ���B�����ҵ�һ���Ǹ���㣬���Ĺؼ��ָ���С��MEDIAN
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
	if(current!=root&&current->data_count<MEDIAN)//���ǵ���B��������
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
		//step4.�ټ��һ�£���Ҫ�������������
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
		left_brother=parent->branch[position-1];//�����ֵ�����
		right_brother=current;
		//�������ֵܽ��
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
		//�������ֵ�
		right_brother->data_count=0;
		//����˫�׽��
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
		//�������ֵ�
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
		//�������ֵ�
		right_brother->data_count=0;
		//����˫�׽��
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
	cout<<"B�������룬ɾ���Ͳ���\n\n"
		<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~B��~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
		 <<"1.ѡ��Ӳ����ļ�����ؼ��ֽ���B��\n"
		 <<"2.ѡ��Ӽ������룬��B������ؼ���\n"
		 <<"3.���B��\n"
		 <<"4.ɾ���ؼ���\n"
		 <<"5.���ҹؼ���\n"
		 <<"6.�˳�\n"
		 <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
	while(cin>>choice)//������keyΪ'a'ʱ��Ӱ�쵽����һ��ĳ���//����
	{
		switch(choice)
		{
		case 1:
			{
				cout<<"ѡ����1���Ӳ����ļ�����\n";
				tree.ReadFile();
			}break;
		case 2:
			{
				cout<<"ѡ����2���Ӽ������롣������ؼ��֣�";
				if(cin>>key)
				{
					if((int)key==key)
					{tree.Insert((int)key);cout<<(int)key<<"�Ѿ���������\n";}
					else cout<<"��������";
				}
				else
					cout<<"��������\n";
			}break;
		case 3:
			{
				cout<<"ѡ����3\n";
				tree.HSAM(tree.GetRoot());
			}break;
		case 4:
			{
				cout<<"ѡ����4.������Ҫɾ���Ĺؼ��֣�";
				if(cin>>key)
				{
					if((int)key==key)
					{
						if(tree.SearchTree((int)key)==Success)
						{
							tree.Delete((int)key);
							cout<<(int)key<<"��ɾ��\n";
						}
						else
							cout<<(int)key<<"�������У��޷�ɾ��\n";
					}
				}
				else
					cout<<"��������\n";				
			}break;
		case 5:
			{
				cout<<"ѡ����5��������Ҫ���ҵĹؼ��֣�";
				if(cin>>key)
				{
					if((int)key==key)
					{
						if(tree.SearchTree((int)key)==Success)
							cout<<(int)key<<"�����B����\n";
						else
							cout<<(int)key<<"�������B����\n";
					}
				}
				else
					cout<<"��������\n";	
			}break;
		case 6:
			{
				return 0;
			}break;
		default :
			cout<<"�����������������������\n";break;
		}
		cout<<"\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~B��~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
		 <<"1.ѡ��Ӳ����ļ�����ؼ��ֽ���B��\n"
		 <<"2.ѡ��Ӽ������룬��B������ؼ���\n"
		 <<"3.���B��\n"
		 <<"4.ɾ���ؼ���\n"
		 <<"5.���ҹؼ���\n"
		 <<"6.�˳�\n"
		 <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
	}
	return 0;
}