#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<math.h>
#define MAXNODE 256+255//Huffman�ܽڵ���
#define MAX 10000000
#define MAXCODE 50
using namespace std;

//Huffman���ڵ�
typedef struct HTNode
{
	int weight;
	int parent;
	int Lchild, Rchild;
}HTNode;

//��ȡԴ�ļ�������ÿ���ֽڣ���¼ÿ���ֽ�Ȩֵ
int readPic(char* srcFile, int* picByte, int* picData)
{
	FILE* fp = NULL;
	if ((fp = fopen(srcFile, "rb")) == NULL)
		return 0;
	int i = 0;
	while (!feof(fp))
	{
		int data = fgetc(fp);//��ȡһ���ֽ���Ϣ�������ͷ��ظ�data
		picByte[i++] = data;//����Ϣ���浽������
		(picData[data])++;//���ֵ��ֽ�Ȩֵ+1
	}
	fclose(fp);
	return i - 1;//����Դ�ļ��ֽ���
}
//������֪�Ľڵ�Ȩֵ����Huffman��
void BuildHuffTree(HTNode* HTree)
{
	int isOK = 1;//��¼�Ƿ񹹽����
	int end = 256;//��¼���ӵ���λ��

	while (isOK) {
		isOK = 0;
		int min1 = MAX;
		int flag1 = 0;
		for (int i = 0;i < end;i++)//�ҵ�һС�ڵ�
		{
			if (HTree[i].parent != -1)//������Ľڵ�����
				continue;
			if (HTree[i].weight < min1)
			{
				min1 = HTree[i].weight;
				flag1 = i;
			}
		}
		HTree[flag1].parent = -2;//��ȥ���ҵ��ĵ�һС�ڵ㣬����Ӱ���ҵڶ�С

		int min2 = MAX;
		int flag2 = 0;
		for (int i = 0;i < end;i++)//�ҵڶ�С�ڵ�
		{
			if (HTree[i].parent != -1)//������Ľڵ�����
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
			HTree[flag1].parent = HTree[flag2].parent = end;//����С�������ڵ����Huffman��
			HTree[end].Lchild = flag1;
			HTree[end].Rchild = flag2;
			HTree[end].weight = HTree[flag1].weight + HTree[flag2].weight;
			end++;
		}
	}
}
//�������Huffman��,����Huffman����
void PreHuffCode(HTNode* HTree, int numAll, char** HuffCode)
{
	for (int i = 0;i < numAll;i++)
		HTree[i].weight = 0;
	char code[256] = { '\0' };//��ʼ������
	int len = 0;//��ʼ�����볤��
	int p = numAll - 1;//���ڵ�

	while (p != -2)//���ͣ�ǰ�湹��Huffman��������ʱ�򣬸��ڵ�ĸ��ڵ���Ϊ��-2
	{
		if (HTree[p].weight == 0)//�����ӽڵ���
		{
			HTree[p].weight = 1;//���Ϊ1��ʾ�ýڵ���ʹ����ӽڵ�
			if (HTree[p].Lchild != -1)
			{
				p = HTree[p].Lchild;
				code[len++] = '0';//�������0
				code[len] = '\0';
			}
			else//û���ӽڵ㣬��Ϊ��Huffman����û�����ӽڵ�϶���û�����ӽڵ�
			{
				strcpy(HuffCode[p], code);
				p = HTree[p].parent;
				if (len > 0)
					code[--len] = '\0';//�������һ����λ
			}
		}
		else if (HTree[p].weight == 1)//�����ӽڵ���
		{
			HTree[p].weight = 2;//���Ϊ2��ʾ�ýڵ���ʹ����ӽڵ�
			if (HTree[p].Rchild != -1)
			{
				p = HTree[p].Rchild;
				code[len++] = '1';//�������1
				code[len] = '\0';
			}
		}
		else//�����ڵ���
		{
			p = HTree[p].parent;
			if (len > 0)
				code[--len] = '\0';
		}
	}
}
//�ļ�ѹ����Ŀ���ļ�
int compress(char* destFile, char** HuffCode, int* srcByte, int srcSize, int isOut)
{
	FILE* fp = NULL;
	if ((fp = fopen(destFile, "wb")) == NULL)
		return 0;

	int* destByte = new int[srcSize * 8];//�洢ѹ���ļ��Ķ�����ֵ����
	int flag = 0;

	int num = 0;//��¼ѹ���ļ�������λ��
	for (int i = 0;i < srcSize - 1;i++)
	{
		int byte = srcByte[i];//Դ�ļ���i���ֽ�

		int len = 0;
		for (int j = 0;j < 256;j++)
		{
			if (HuffCode[byte][j] == '0' || HuffCode[byte][j] == '1')//ȷ�����볤��
			{
				destByte[flag++] = HuffCode[byte][j] - '0';
				len++;
			}
			else
				break;
		}
		if (isOut)//�Ƿ����
		{
			for (int k = 0;k < len;k++)
				cout << HuffCode[byte][k];
		}
		num += len;
	}
	cout << endl;

	int byte = 0;
	for (int i = 0;i < num;i++)//��ѹ�����밴�ֽ�д��ѹ���ļ�
	{
		byte += pow(2, 7 - i % 8) * destByte[i];
		if ((i + 1) % 8 == 0)
		{
			char a = (char)byte;
			fwrite(&a, sizeof(a), 1, fp);
			byte = 0;
		}
	}
	fclose(fp);//�ر��ļ�
	return num/8 + num % 8;//����ѹ������ֽ���
}

//��ʼ��
void init_int(int* picData)
{
	for (int i = 0;i < 256;i++)
		picData[i] = 0;
}
//��ʼ��
void init_node(HTNode* HTree, int* picData)
{
	for (int i = 0;i < MAXNODE;i++)//��ʼ��
	{
		if (i < 256)
		{
			HTree[i].weight = picData[i];//���ֽڵ�Ȩֵת�浽Huffman�ڵ�Ȩֵ��
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
	char* srcFile = new char[50];//�洢Դ�ļ�·�����ַ���
	cout << "�������ļ�����";
	cin >> srcFile;

	int* srcByte = new int[MAX];//�洢Դ�ļ����������ݵ�����
	int srcSize = 0;//�洢Դ�ļ��ֽ���

	int* srcData = new int[256];//�洢Դ�ļ�������Ȩֵ������
	init_int(srcData);

	if (!(srcSize = readPic(srcFile, srcByte, srcData)))
	{
		cout << "ͼƬ��ȡʧ�ܣ�" << endl;
		return 0;
	}

	HTNode* HTree = new HTNode[MAXNODE];//Huffman���ڵ�����
	init_node(HTree, srcData);//��ʼ���ڵ�

	cout << "�������Huffman���Ĺ���" << endl;
	BuildHuffTree(HTree);//����Huffman��
	cout << "�ֽ���� " << "Ȩֵ\t" << "���ڵ�\t" << "���ӽڵ� " << "���ӽڵ�" << endl;
	test(HTree);

	char** HuffCode = new char* [256];//�洢�ֽڱ�����ַ�����
	for (int i = 0;i < 256;i++)
		HuffCode[i] = new char[MAXCODE];

	cout << "�������Huffman����" << endl;
	PreHuffCode(HTree, MAXNODE, HuffCode);//���ݹ����õ�Huffman������Huffman����
	for (int i = 0;i < 256;i++)//���������
		cout << i << ":" << HuffCode[i] << endl;

	cout << "������ɣ�" << endl;
	cout << "��ʼѹ��" << endl;

	int isOut;
	cout << "���Ƿ�Ҫ����ļ���ѹ������?(1:�ǣ�0:��)" << endl;
	cin >> isOut;

	char destFile[] = "D:/destFile.huf";
	int destSize = 0;//��¼Ŀ���ļ��ֽ���
	if (!(destSize = compress(destFile, HuffCode, srcByte, srcSize, isOut)))
	{
		cout << "ѹ���ļ�·�������ڣ�ѹ��ʧ�ܣ�" << endl;
		return 0;
	}

	cout << "ԭ�ļ���ռ�ֽ�����" << srcSize << "�ֽ�" << endl;
	cout << "ѹ���ļ���ռ�ֽ�����" << destSize << "�ֽ�" << endl;
	double rate = destSize / (double)srcSize;//�������
	printf("ѹ������Ϊ��%.4lf%%\n", 100 * rate);

	return 0;
}