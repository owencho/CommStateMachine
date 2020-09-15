#ifndef CMDNODE_H
#define CMDNODE_H

typedef struct CmdNode CmdNode;
struct CmdNode{
    CmdNode * left ;
    CmdNode * right ;
    int bFactor;
    int command;
	void * info;
};

#endif // CMDNODE_H
