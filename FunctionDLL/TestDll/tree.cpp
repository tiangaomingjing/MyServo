﻿#include "tree.h"

//***** 下面是对于TreeNode结构体的定义实现*****///

TreeNode::TreeNode(int type= 0,TreeNode* Parent = 0)
{
 _data = type;
 _parent = Parent;
}
void TreeNode::SetParent(TreeNode& node)
{
 _parent = &node;
}
void TreeNode::InsertChildren(TreeNode& node)
{
 TreeNode* p = &node;
 _children.push_back(p);
}



//***** 下面是对于Tree类的定义实现*****///
Tree::Tree()
{

}

Tree::Tree(const int type)
{
 _nodes.push_back(new TreeNode(type));
}

Tree::Tree(const Tree& t)
{
 if(t._nodes.empty())return;
 clone(t._nodes.front(),_nodes,0);
}
Tree::Tree(const int type,const list<Tree*>& lit)
{
 TreeNode* root = new TreeNode(type);//建立根节点
 _nodes.push_back(root);//放入树中
 list<Tree*>::const_iterator it;
 for(it = lit.begin();it!=lit.end();it++)
 {
   if(!((*it)->_nodes.empty()))
   {//如果当前节点元素不为空
     Tree* tp = new Tree(**it);
     TreeNode* p = tp->_nodes.front();
     root->_children.push_back(p); //设置根的子节点
     p->_parent = root;            //设置节点的父节点为根
     list<TreeNode*>::iterator lit1 = tp->_nodes.begin();
     list<TreeNode*>::iterator lit2 = tp->_nodes.end();
     list<TreeNode*>::iterator lit3 = _nodes.end();
     _nodes.insert(lit3,lit1,lit2);
   }
 }
}

Tree::~Tree()
{
 for(list<TreeNode*>::iterator it = _nodes.begin();it!=_nodes.end();it++)
 {
 delete* it;
 }
}

Tree& Tree::operator =(const Tree & t)
{
 Clear();
 Tree* p = new Tree(t);
 _nodes = p->_nodes;
 return *this;
}

bool Tree::operator ==(const Tree& t)
{
 if(_nodes.size()!=t._nodes.size())
 {
 return false;
 }
 list<TreeNode*>::iterator it = _nodes.begin();
 list<TreeNode*>::const_iterator _it = t._nodes.begin();
 while(it!=_nodes.end()&&_it!=t._nodes.end())
 {
 if((*it)->_data!=(*_it)->_data)
 {
   return false;
 }
 it++;
 _it++;
 }
 return true;
}

bool Tree::operator !=(const Tree& t)
{
 if(_nodes.size()!=_nodes.size())
 {
 return true;
 }
 else
 {
 list<TreeNode*>::iterator it = _nodes.begin();
     list<TreeNode*>::const_iterator _it = t._nodes.begin();
 while(it!=_nodes.end()&&_it!=t._nodes.end())
 {
   if((*it)->_data!=(*_it)->_data)
   {
    return true;
   }
   it++;
   _it++;
 }
 return false;
 }
}

void Tree::Clear()
{
 for(list<TreeNode*>::iterator it = _nodes.begin();it!=_nodes.end();it++)
 {
  delete* it;
 }
 _nodes.clear();
}

bool Tree::IsEmpty()const
{
 return _nodes.empty();
}

int Tree::Size()const
{
 return (int)_nodes.size();
}

int Tree::Leaves()
{
 int i = 0;
 list<TreeNode*>::iterator it = _nodes.begin();
 while(it!=_nodes.end())
 {
 if((*it)->_children.size()==0)
 {
   i++;
 }
 it++;
 }
 return i;
}


int Tree::Height()
{
 if(_nodes.size()!=0)
 {
 TreeNode* TNode = _nodes.front();
 return height(TNode);
 }
 else
 {
 return -1; //判断为空树
 }
}

int Tree::height(TreeNode* node)
{
 if(!node)
 {
 return -1;
 }
 else
 {
 list<TreeNode*> plist = node->_children;
 if(plist.size()==0){
   return 0;
 }
 int hA = 0;
 for(list<TreeNode*>::iterator it = plist.begin();it!=plist.end();it++)
 {
  int hB = height(*it);
   if(hB>hA)
   {
    hA = hB;
   }
 }
 return hA+1;
 }
}


Iterator Tree::begin()
{
 return Iterator(this,_nodes.begin());
}

Iterator Tree::end()
{
 return Iterator(this,_nodes.end());
}
int Tree::Root()const
{
 return (*_nodes.begin())->_data;
}


bool Tree::IsRoot(Iterator it)
{
 TreeNode p = *it;
 if(p._parent == 0)
 {
 return true;
 }
 return false;
}

bool Tree::isLeaf(Iterator it)
{
 TreeNode p = *it;
 if(p._children.size() == 0)
 {
 return true;
 }
 return false;
}

Iterator Tree::Parent(Iterator it)
{
 TreeNode p = *it;
 Tree* t = it._tree;
 Iterator Ite(t,p._parent);
 return Ite;
}


int Tree::NumChildren(Iterator it)
{
 TreeNode p = *it;
 return (int)p._children.size();
}

//***** 下面是对于Tree::Iterator类的定义实现*****///
Iterator::Iterator()
{
}

Iterator::Iterator(const Iterator& it)
{
 _tree = it._tree;
 _lit = it._lit;
}

Iterator::Iterator(Tree* t, TreeNode* n)
{
 _tree = t;
 list<TreeNode*>& nodes = _tree->_nodes;
 _lit = find(nodes.begin(),nodes.end(),n);//<algorithm> Members
}

Iterator::Iterator(Tree * t, list<TreeNode*>::iterator lt)
{
 _tree = t;
 _lit = lt;
}

void Iterator::operator =(const Iterator& it)
{
 _tree = it._tree;
 _lit = it._lit;
}

bool Iterator::operator ==(const Iterator & it)
{
 return _tree == it._tree && _lit == it._lit;
}

bool Iterator::operator !=(const Iterator & it)
{
 return _tree != it._tree || _lit != it._lit;
}

Iterator& Iterator::operator ++()
{
 ++_lit;
 return *this;
}

Iterator Iterator::operator ++(int)
{
 Iterator it(*this);
 ++_lit;
 return it;
}

int Iterator::operator *() const
{
 return ((*_lit)->_data);
}

bool Iterator::operator !()
{
 return _lit == _tree->_nodes.end();
}

//Clone函数
TreeNode* clone(TreeNode* node,List& nodes,TreeNode* nodep)
{
 TreeNode* cp = new TreeNode(node->_data,nodep);
 nodes.push_back(cp);
 List& l = node->_children;
 List& cl = cp->_children;
 for(list<TreeNode*>::iterator lt = l.begin();lt!=l.end();lt++)
 {
 cl.push_back(clone(*lt,nodes,cp));
 }
 return cp;
}
