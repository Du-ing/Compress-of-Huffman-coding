#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<math.h>
#define MAXNODE 256+255//Huffman总节点数
#define MAX 10000000
#define MAXCODE 50
using namespace std;

//Huffman树节点
typedef struct HTNode
{
	int weight;
	int parent;
	int Lchild, Rchild;
}HTNode;

//读取源文件，保存每个字节，记录每个字节权值
int readPic(char* srcFile, int* picByte, int* picData)
{
	FILE* fp = NULL;
	if ((fp = fopen(srcFile, "rb")) == NULL)
		return 0;
	int i = 0;
	while (!feof(fp))
	{
		int data = fgetc(fp);//读取一个字节信息，以整型返回给data
		picByte[i++] = data;//将信息保存到数组中
		(picData[data])++;//出现的字节权值+1
	}
	fclose(fp);
	return i - 1;//返回源文件字节数
}
//利用已知的节点权值构建Huffman树
void BuildHuffTree(HTNode* HTree)
{
	int isOK = 1;//记录是否构建完成
	int end = 256;//记录增加到的位置

	while (isOK) {
		isOK = 0;
		int min1 = MAX;
		int flag1 = 0;
		for (int i = 0;i < end;i++)//找第一小节点
		{
			if (HTree[i].parent != -1)//构造过的节点跳过
				continue;
			if (HTree[i].weight < min1)
			{
				min1 = HTree[i].weight;
				flag1 = i;
			}
		}
		HTree[flag1].parent = -2;//先去掉找到的第一小节点，避免影响找第二小

		int min2 = MAX;
		int flag2 = 0;
		for (int i = 0;i < end;i++)//找第二小节点
		{
			if (HTree[i].parent != -1)//构造过的节点跳过
				continue;
			if (HTree[i].weight < min2)
			{
				isOK = 1;
				min2 = HTree[i].weight;
				flag2 = i;
			}
		}
		if (isOK)
		{
			HTree[flag1].parent = HTree[flag2].parent = end;//将最小的两个节点加入Huffman树
			HTree[end].Lchild = flag1;
			HTree[end].Rchild = flag2;
			HTree[end].weight = HTree[flag1].weight + HTree[flag2].weight;
			end++;
		}
	}
}
//先序遍历Huffman树,进行Huffman编码
void PreHuffCode(HTNode* HTree, int numAll, char** HuffCode)
{
	for (int i = 0;i < numAll;i++)
		HTree[i].weight = 0;
	char code[256] = { '\0' };//初始化编码
	int len = 0;//初始化编码长度
	int p = numAll - 1;//根节点

	while (p != -2)//解释：前面构建Huffman树结束的时候，根节点的父节点设为了-2
	{
		if (HTree[p].weight == 0)//向左子节点走
		{
			HTree[p].weight = 1;//标记为1表示该节点访问过左子节点
			if (HTree[p].Lchild != -1)
			{
				p = HTree[p].Lchild;
				code[len++] = '0';//编码加上0
				code[len] = '\0';
			}
			else//没有子节点，因为在Huffman树中没有左子节点肯定就没有右子节点
			{
				strcpy(HuffCode[p], code);
				p = HTree[p].parent;
				if (len > 0)
					code[--len] = '\0';//编码减少一个单位
			}
		}
		else if (HTree[p].weight == 1)//向右子节点走
		{
			HTree[p].weight = 2;//标记为2表示该节点访问过右子节点
			if (HTree[p].Rchild != -1)
			{
				p = HTree[p].Rchild;
				code[len++] = '1';//编码加上1
				code[len] = '\0';
			}
		}
		else//往父节点走
		{
			p = HTree[p].parent;
			if (len > 0)
				code[--len] = '\0';
		}
	}
}
//文件压缩到目标文件
int compress(char* destFile, char** HuffCode, int* srcByte, int srcSize, int isOut)
{
	FILE* fp = NULL;
	if ((fp = fopen(destFile, "wb")) == NULL)
		return 0;

	int* destByte = new int[srcSize * 8];//存储压缩文件的二进制值数据
	int flag = 0;

	int num = 0;//记录压缩文件编码总位数
	for (int i = 0;i < srcSize - 1;i++)
	{
		int byte = srcByte[i];//源文件第i个字节

		int len = 0;
		for (int j = 0;j < 256;j++)
		{
			if (HuffCode[byte][j] == '0' || HuffCode[byte][j] == '1')//确定编码长度
			{
				destByte[flag++] = HuffCode[byte][j] - '0';
				len++;
			}
			else
				break;
		}
		if (isOut)//是否输出
		{
			for (int k = 0;k < len;k++)
				cout << HuffCode[byte][k];
		}
		num += len;
	}
	cout << endl;

	int byte = 0;
	for (int i = 0;i < num;i++)//将压缩编码按字节写入压缩文件
	{
		byte += pow(2, 7 - i % 8) * destByte[i];
		if ((i + 1) % 8 == 0)
		{
			char a = (char)byte;
			fwrite(&a, sizeof(a), 1, fp);
			byte = 0;
		}
	}
	fclose(fp);//关闭文件
	return num/8 + num % 8;//返回压缩后的字节数
}

//初始化
void init_int(int* picData)
{
	for (int i = 0;i < 256;i++)
		picData[i] = 0;
}
//初始化
void init_node(HTNode* HTree, int* picData)
{
	for (int i = 0;i < MAXNODE;i++)//初始化
	{
		if (i < 256)
		{
			HTree[i].weight = picData[i];//将字节的权值转存到Huffman节点权值中
		}
		else
		{
			HTree[i].weight = 0;
		}
		HTree[i].parent = HTree[i].Lchild = HTree[i].Rchild = -1;
	}
}
void test(HTNode* HTree)
{
	for (int i = 0;i < 256 + 255;i++)
		cout << i << "\t " << HTree[i].weight << "\t" << HTree[i].parent << "\t" << HTree[i].Lchild << "\t " << HTree[i].Rchild << endl;
}

int main()
{
	char* srcFile = new char[50];//存储源文件路径的字符串
	cout << "请输入文件名：";
	cin >> srcFile;

	int* srcByte = new int[MAX];//存储源文件二进制数据的数组
	int srcSize = 0;//存储源文件字节数

	int* srcData = new int[256];//存储源文件二进制权值的数组
	init_int(srcData);

	if (!(srcSize = readPic(srcFile, srcByte, srcData)))
	{
		cout << "图片读取失败！" << endl;
		return 0;
	}

	HTNode* HTree = new HTNode[MAXNODE];//Huffman树节点数组
	init_node(HTree, srcData);//初始化节点

	cout << "下面进行Huffman树的构造" << endl;
	BuildHuffTree(HTree);//构建Huffman数
	cout << "字节序号 " << "权值\t" << "父节点\t" << "左子节点 " << "右子节点" << endl;
	test(HTree);

	char** HuffCode = new char* [256];//存储字节编码的字符数组
	for (int i = 0;i < 256;i++)
		HuffCode[i] = new char[MAXCODE];

	cout << "下面进行Huffman编码" << endl;
	PreHuffCode(HTree, MAXNODE, HuffCode);//根据构建好的Huffman树进行Huffman编码
	for (int i = 0;i < 256;i++)//输出编码结果
		cout << i << ":" << HuffCode[i] << endl;

	cout << "编码完成！" << endl;
	cout << "开始压缩" << endl;

	int isOut;
	cout << "您是否要输出文件的压缩编码?(1:是；0:否)" << endl;
	cin >> isOut;

	char destFile[] = "D:/destFile.huf";
	int destSize = 0;//记录目标文件字节数
	if (!(destSize = compress(destFile, HuffCode, srcByte, srcSize, isOut)))
	{
		cout << "压缩文件路径不存在，压缩失败！" << endl;
		return 0;
	}

	cout << "原文件所占字节数：" << srcSize << "字节" << endl;
	cout << "压缩文件所占字节数：" << destSize << "字节" << endl;
	double rate = destSize / (double)srcSize;//计算比率
	printf("压缩比率为：%.4lf%%\n", 100 * rate);

	return 0;
}