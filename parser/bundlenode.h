#ifndef BUNDLENODE_H
#define BUNDLENODE_H

#include <string>

#include "ast.h"
#include "listnode.h"

class BundleNode : public AST
{
public:
    BundleNode(string name, ListNode *indexList, int line);
    BundleNode(string name, string namespace_, ListNode *indexList, int line);
    virtual ~BundleNode();

    string getName() const;
    string getNamespace() const;
    ListNode *index() const;

    AST *deepCopy();

private:
    string m_name;
    string m_namespace;
};

#endif // BUNDLENODE_H
