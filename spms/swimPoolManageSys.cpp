//游泳池客户管理系统
//26.7.31
//by 筝羽, Laia, prince

//26.8.1 v0.1.1:新增管理员系统



#include<stdio.h>
#include<string.h>
#include<time.h>
#include <stdlib.h>
#include <windows.h>

// 管理员
struct Admin {
    int id;            // 编号
    char account[30];  // 账号
    char password[30]; // 加密后的密码
};

//会员信息
struct VIP {
    int cardID;         //卡号
    char name[30];      //姓名
    char phone[20];     //手机号
    char gender[5];     //性别
    double balance;     //余额
    int status;         //是否挂失
};

//登记(日志)
struct log {
    int cardID;
    char name[30];
    char phone[20];
    char enTime[20];    //进入时间
    char exTime[20];    //离开时间
    double stHours;     //停留时长(h)
    double charge;      //收费金额
};

//财务
struct get_money {
    char date[20];      //日期
    double charge;
    char source[30];    //来源
};

//数组空间
#define MAX_ADMINS 20
#define MAX_VIPS 150
#define MAX_LOGS 500
#define MAX_GET_MONEY 500
#define PRICE_PER_HOUR 20.0

struct Admin admins[MAX_ADMINS];
struct VIP vips[MAX_VIPS];
struct log logs[MAX_LOGS];
struct get_money money[MAX_GET_MONEY];

//计数
int adminCount = 0;
int VIPCount = 0;
int logCount = 0;
int moneyCount = 0;

//菜单页面
void mainMenu();
void VIPMenu();
void logMenu();
void moneyMenu();
void loginMenu();

//管理员操作
void readAdminFile();
void saveAdminFile();
void encryptPassword(char* src, char* dst);
void decryptPassword(char* src, char* dst);
int verifyAdmin(char* account, char* password);
int verifyAnyAdmin();
void adminLogin();
void addAdmin();
void deleteAdmin();

//会员操作
void addVIP();
void deleteVIP();
void changeVIP();
void searchVIP();
void sortVIP();
void listVIP();

//文件读写
void readFile();
void saveFile();

//登记操作 
void addLog();
void checkOut();
void listLog();

//财务操作 
void addMoney();
void listFinance();
void financeSummary();

//=================================================函数实现
//主函数
int main() {
    char path[256];
    GetModuleFileNameA(NULL, path, 256);
    char* p = strrchr(path, '\\');
    if (p) {
        *p = '\0';
        SetCurrentDirectoryA(path);
    }

    readAdminFile();
    loginMenu();

    readFile();

    int n;
    char input[10];
    do {
        system("cls");
        mainMenu();
        scanf("%s", input);
        while (getchar() != '\n');

        n = atoi(input);

        switch (n) {
        case 1:VIPMenu(); break;
        case 2:logMenu(); break;
        case 3:moneyMenu(); break;
        case 0:printf("退出系统\n"); break;
        default:printf("无效输入!\n");Sleep(500);
        }
    } while (n != 0);

    saveFile();
    saveAdminFile();
    return 0;
}

//--------------------------菜单
void mainMenu() {
    printf("\n------游泳馆客户管理系统------\n");
    printf("|         1.会员管理         |\n");
    printf("|         2.登记管理         |\n");
    printf("|         3.财务管理         |\n");
    printf("|         0.退出系统         |\n");
    printf("------------------------------\n");
    printf("\n    <输入对应序号执行操作>\n");
}

void VIPMenu() {
    int choice;
    char input[10];
    do {
        system("cls");
        printf("\n--- 会员管理 ---\n");
        printf("1. 新增会员\n");
        printf("2. 删除会员\n");
        printf("3. 修改会员\n");
        printf("4. 查询会员\n");
        printf("5. 排序会员\n");
        printf("6. 列出所有会员\n");
        printf("7. 保存数据到文件\n");
        printf("0. 返回主菜单\n");
        printf("请选择：");
        scanf("%s", input);
        while (getchar() != '\n');

        choice = atoi(input);

        switch (choice) {
        case 1: addVIP(); break;
        case 2: deleteVIP(); break;
        case 3: changeVIP(); break;
        case 4: searchVIP(); break;
        case 5: sortVIP(); break;
        case 6: listVIP(); break;
        case 7: saveFile(); break;
        case 0: printf("返回主菜单\n"); break;
        default: printf("无效输入！\n");Sleep(500);
        }
    } while (choice != 0);
}

void logMenu() {
    int choice;
    char input[10];
    do {
        system("cls");
        printf("\n--- 登记管理 ---\n");
        printf("1. 新增入场登记\n");
        printf("2. 离场结算\n");
        printf("3. 列出所有登记记录\n");
        printf("4. 保存数据到文件\n");
        printf("0. 返回主菜单\n");
        printf("请选择：");
        scanf("%s", input);
        while (getchar() != '\n');

        choice = atoi(input);

        switch (choice) {
        case 1:addLog(); break;
        case 2:checkOut(); break;
        case 3:listLog(); break;
        case 4:saveFile(); break;
        case 0:printf("返回主菜单\n"); break;
        default:
            printf("无效输入！\n");Sleep(500);
        }
    } while (choice != 0);
}

void moneyMenu() {
    int choice;
    char input[10];
    do {
        system("cls");
        printf("\n--- 财务管理 ---\n");
        printf("1. 查看所有财务记录\n");
        printf("2. 统计总收入\n");
        printf("3. 手动添加收入\n");
        printf("4. 保存数据到文件\n");
        printf("0. 返回主菜单\n");
        printf("请选择：");
        scanf("%s", input);
        while (getchar() != '\n');

        choice = atoi(input);

        switch (choice) {
        case 1: listFinance(); break;
        case 2: financeSummary(); break;
        case 3: addMoney(); break;
        case 4: saveFile(); break;
        case 0: printf("返回主菜单\n"); break;
        default: printf("无效输入！\n");Sleep(500);
        }
    } while (choice != 0);
}

//----------------------------管理员操作 

// 加密
void encryptPassword(char* src, char* dst) {
    while (*src) {
        *dst = *src + 1;
        src++;
        dst++;
    }
    *dst = '\0';
}

// 解密
void decryptPassword(char* src, char* dst) {
    while (*src) {
        *dst = *src - 1;
        src++;
        dst++;
    }
    *dst = '\0';
}

// 读取文件
void readAdminFile() {
    FILE* fp = fopen("admins.txt", "r");
    if (fp == NULL) {
        adminCount = 0;
        admins[0].id = 1;
        strcpy(admins[0].account, "root");
        char defaultPwd[] = "admin";
        encryptPassword(defaultPwd, admins[0].password);
        adminCount = 1;
        saveAdminFile();
        return;
    }

    adminCount = 0;
    while (adminCount < MAX_ADMINS &&
        fscanf(fp, "%d\t%[^\t]\t%s\n",
            &admins[adminCount].id,
            admins[adminCount].account,
            admins[adminCount].password) == 3) {
        adminCount++;
    }
    fclose(fp);

    // 创建默认
    if (adminCount == 0) {
        admins[0].id = 1;
        strcpy(admins[0].account, "root");
        char defaultPwd[] = "admin";
        encryptPassword(defaultPwd, admins[0].password);
        adminCount = 1;
        saveAdminFile();
    }
}

// 保存
void saveAdminFile() {
    FILE* fp = fopen("admins.txt", "w");
    if (fp == NULL) {
        printf("无法保存管理员文件！\n");
        return;
    }
    for (int i = 0; i < adminCount; i++) {
        fprintf(fp, "%d\t%s\t%s\n",
            admins[i].id,
            admins[i].account,
            admins[i].password);
    }
    fclose(fp);
}

// 验证管理员
int verifyAdmin(char* account, char* password) {
    char encrypted[30];
    encryptPassword(password, encrypted);
    for (int i = 0; i < adminCount; i++) {
        if (strcmp(admins[i].account, account) == 0 &&
            strcmp(admins[i].password, encrypted) == 0) {
            return i;
        }
    }
    return -1;
}

// 验证
int verifyAnyAdmin() {
    char account[30], password[30];
    printf("请输入管理员账号：");
    scanf("%s", account);
    printf("请输入管理员密码：");
    scanf("%s", password);

    int idx = verifyAdmin(account, password);
    if (idx == -1) {
        printf("账号或密码错误！\n");
        Sleep(2000);
        return 0;
    }
    return 1;
}

void loginMenu() {
    int choice;
    char input[10];
    do {
        system("cls");
        printf("\n========= 管理员登录 =========\n");
        printf("1. 登录系统\n");
        printf("2. 新建管理员账户\n");
        printf("3. 删除管理员账户\n");
        printf("0. 退出程序\n");
        printf("===============================\n");
        printf("请选择：");
        scanf("%s", input);
        while (getchar() != '\n');  // 清空缓冲区

        choice = atoi(input);

        switch (choice) {
        case 1:
            adminLogin();
            return;  // 登录成功
        case 2:
            addAdmin();
            break;
        case 3:
            deleteAdmin();
            break;
        case 0:
            printf("退出程序\n");
            Sleep(1000);
            exit(0);
        default:
            printf("无效输入！\n");
            Sleep(2000);
        }
    } while (1);
}

void adminLogin() {
    char account[30], password[30];
    printf("请输入账号：");
    scanf("%s", account);
    printf("请输入密码：");
    scanf("%s", password);

    if (verifyAdmin(account, password) != -1) {
        printf("登录成功！\n");
        Sleep(1000);
        return;
    }
    else {
        printf("账号或密码错误！\n");
        Sleep(2000);
        loginMenu();  // 返回登录菜单
    }
}

void addAdmin() {
    if (adminCount >= MAX_ADMINS) {
        printf("管理员数量已达上限（%d人）！\n", MAX_ADMINS);
        Sleep(2000);
        return;
    }

    // 验证
    if (adminCount > 0) {
        printf("需要管理员权限才能新建账户！\n");
        if (!verifyAnyAdmin()) {
            return;
        }
    }
    else {
        printf("当前无管理员，使用默认账户 root/admin 创建首个管理员。\n");
    }

    char newAccount[30], newPassword[30];
    printf("请输入新管理员账号：");
    scanf("%s", newAccount);

    // 检查账号
    for (int i = 0; i < adminCount; i++) {
        if (strcmp(admins[i].account, newAccount) == 0) {
            printf("该账号已存在！\n");
            Sleep(2000);
            return;
        }
    }

    printf("请输入新管理员密码：");
    scanf("%s", newPassword);

    // 生成编号
    int maxId = 0;
    for (int i = 0; i < adminCount; i++) {
        if (admins[i].id > maxId) maxId = admins[i].id;
    }

    admins[adminCount].id = maxId + 1;
    strcpy(admins[adminCount].account, newAccount);
    encryptPassword(newPassword, admins[adminCount].password);
    adminCount++;
    saveAdminFile();

    printf("管理员创建成功！编号：%d\n", maxId + 1);
    Sleep(2000);
}

void deleteAdmin() {
    if (adminCount <= 1) {
        printf("至少保留一个管理员账户！\n");
        Sleep(2000);
        return;
    }

    printf("需要管理员权限才能删除账户！\n");
    if (!verifyAnyAdmin()) {
        return;
    }

    char delAccount[30];
    printf("请输入要删除的管理员账号：");
    scanf("%s", delAccount);

    // 禁止删除 root
    if (strcmp(delAccount, "root") == 0) {
        printf("不能删除默认管理员 root！\n");
        Sleep(2000);
        return;
    }

    int index = -1;
    for (int i = 0; i < adminCount; i++) {
        if (strcmp(admins[i].account, delAccount) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("未找到该管理员账号！\n");
        Sleep(2000);
        return;
    }

    // 删除
    for (int i = index; i < adminCount - 1; i++) {
        admins[i] = admins[i + 1];
    }
    adminCount--;
    saveAdminFile();

    printf("管理员 %s 已删除！\n", delAccount);
    Sleep(2000);
}

// ---------------------------------会员操作
void addVIP() {
    //检查数组
    if (VIPCount >= MAX_VIPS) {
        printf("会员已满（最多%d人），无法继续添加！\n", MAX_VIPS);
        Sleep(2000);
        return;
    }

    int newCardID;
    printf("请输入卡号（8位及以内整数，0代表非会员）：");
    scanf("%d", &newCardID);

    //检查卡号
    if (newCardID < 1 || newCardID > 99999999) {
        printf("卡号必须小于八位且不能为0！\n");
        Sleep(2000);
        return;
    }

    for (int i = 0; i < VIPCount; i++) {
        if (vips[i].cardID == newCardID) {
            printf("该卡号(%d)已存在，添加失败！\n", newCardID);
            Sleep(2000);
            return;
        }
    }

    //录入数据
    vips[VIPCount].cardID = newCardID;

    printf("请输入姓名：");
    scanf("%s", vips[VIPCount].name);

    printf("请输入手机号：");
    scanf("%s", vips[VIPCount].phone);

    printf("请输入性别（男/女）：");
    scanf("%s", vips[VIPCount].gender);

    printf("请输入首次充值金额：");
    scanf("%lf", &vips[VIPCount].balance);

    //默认状态
    vips[VIPCount].status = 0;

    //会员数量增加
    VIPCount++;

    printf(" 会员添加成功！当前共有 %d 位会员。\n", VIPCount);
    Sleep(2000);
}

void deleteVIP() {
    if (VIPCount == 0) {
        printf("当前无任何会员信息，无法删除！\n");
        Sleep(2000);
        return;
    }

    int targetID;
    printf("请输入要删除的会员卡号：");
    scanf("%d", &targetID);

    // 查找
    int index = -1;
    for (int i = 0; i < VIPCount; i++) {
        if (vips[i].cardID == targetID) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("未找到卡号为 %d 的会员！\n", targetID);
        Sleep(2000);
        return;
    }

    // 删除
    for (int i = index; i < VIPCount - 1; i++) {
        vips[i] = vips[i + 1];
    }
    VIPCount--;

    printf("卡号为 %d 的会员已删除。当前共有 %d 位会员。\n", targetID, VIPCount);
    Sleep(2000);
}

void changeVIP() {
    if (VIPCount == 0) {
        printf("当前无任何会员信息，无法修改！\n");
        Sleep(2000);
        return;
    }

    int targetID;
    printf("请输入要修改的会员卡号：");
    scanf("%d", &targetID);

    int index = -1;
    for (int i = 0; i < VIPCount; i++) {
        if (vips[i].cardID == targetID) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("未找到卡号为 %d 的会员！\n", targetID);
        Sleep(2000);
        return;
    }

    printf("当前信息：\n");
    printf("姓名：%s\n", vips[index].name);
    printf("手机号：%s\n", vips[index].phone);
    printf("性别：%s\n", vips[index].gender);
    printf("余额：%.2f\n", vips[index].balance);
    printf("状态：%s\n", (vips[index].status == 0) ? "正常" : "挂失");

    printf("请输入新的姓名：");
    scanf("%s", vips[index].name);
    printf("请输入新的手机号：");
    scanf("%s", vips[index].phone);
    printf("请输入新的性别（男/女）：");
    scanf("%s", vips[index].gender);
    printf("请输入新的余额：");
    scanf("%lf", &vips[index].balance);
    printf("请输入新状态（0=正常，1=挂失）：");
    scanf("%d", &vips[index].status);

    printf("会员信息已更新！\n");
    Sleep(2000);
}

void searchVIP() {
    if (VIPCount == 0) {
        printf("当前无任何会员信息！\n");
        Sleep(2000);
        return;
    }

    int targetID;
    printf("请输入要查询的会员卡号：");
    scanf("%d", &targetID);

    for (int i = 0; i < VIPCount; i++) {
        if (vips[i].cardID == targetID) {
            printf("\n======== 会员信息 ========\n");
            printf("卡号：%08d\n", vips[i].cardID);
            printf("姓名：%s\n", vips[i].name);
            printf("手机号：%s\n", vips[i].phone);
            printf("性别：%s\n", vips[i].gender);
            printf("余额：%.2f\n", vips[i].balance);
            printf("状态：%s\n", (vips[i].status == 0) ? "正常" : "挂失");
            printf("==========================\n");
            Sleep(3000);
            return;
        }
    }
    printf("未找到卡号为 %d 的会员！\n", targetID);
    Sleep(2000);
}

int judge(struct VIP a, struct VIP b, int choice) {
    switch (choice) {
    case 1:
        return a.cardID > b.cardID;
    case 2:
        return strcmp(a.name, b.name) > 0;
    case 3:
        return a.balance > b.balance;
    case 4:
        return a.status > b.status;
    default:
        return 0;//不排序 
    }
}

void sortVIP() {
    int i, j;
    if (VIPCount == 0) {//没有会员，生意凉凉~
        printf("当前无任何会员信息，无法排序！\n");
        Sleep(2000);
        return;
    }
    int choice;
    printf("请选择排序方式：\n");
    printf("1. 按卡号升序\n");
    printf("2. 按姓名升序\n");
    printf("3. 按余额升序\n");
    printf("4. 按挂失状态排序\n");
    printf("请输入选择：");
    scanf("%d", &choice);
    for (i = 0; i < VIPCount - 1; i++) {
        for (j = 0; j < VIPCount - 1 - i; j++) {
            if (judge(vips[j], vips[j + 1], choice)) {
                struct VIP temp = vips[j];
                vips[j] = vips[j + 1];
                vips[j + 1] = temp;
            }
        }
    }
    listVIP();
}

void listVIP() {
    if (VIPCount == 0) {
        printf("当前无任何会员信息！\n");
        Sleep(2000);
        return;
    }

    printf("\n======================== 会员列表 ========================\n");
    printf("%-8s %-12s %-15s %-4s %-10s %-8s\n",
        "卡号", "姓名", "手机号", "性别", "余额", "状态");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < VIPCount; i++) {
        const char* statusStr = (vips[i].status == 0) ? "正常" : "挂失";
        printf("%08d %-12s %-15s %-4s %-10.2f %-8s\n",
            vips[i].cardID,
            vips[i].name,
            vips[i].phone,
            vips[i].gender,
            vips[i].balance,
            statusStr);
    }
    printf("==========================================================\n");
    Sleep(5000);
}

//--------------------文件读写
void readFile() {
    FILE* fp;
    int i = 0;

    //会员
    fp = fopen("vips.txt", "r");
    if (fp != NULL) {
        VIPCount = 0;
        while (VIPCount < MAX_VIPS &&
            fscanf(fp, "%d\t%[^\t]\t%[^\t]\t%[^\t]\t%lf\t%d\n",
                &vips[VIPCount].cardID,
                vips[VIPCount].name,
                vips[VIPCount].phone,
                vips[VIPCount].gender,
                &vips[VIPCount].balance,
                &vips[VIPCount].status) == 6) {
            VIPCount++;
        }
        fclose(fp);
        printf(" 已读取会员数据 (%d条)\n", VIPCount);
    }
    else {
        // 文件不存在
        printf(" 未找到会员文件，首次运行将新建。\n");
    }

    //登记
    fp = fopen("logs.txt", "r");
    if (fp != NULL) {
        logCount = 0;
        while (logCount < MAX_LOGS &&
            fscanf(fp, "%d\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%lf\t%lf\n",
                &logs[logCount].cardID,
                logs[logCount].name,
                logs[logCount].phone,
                logs[logCount].enTime,
                logs[logCount].exTime,
                &logs[logCount].stHours,
                &logs[logCount].charge) == 7) {
            logCount++;
        }
        fclose(fp);
        printf(" 已读取日志数据 (%d条)\n", logCount);
    }
    else {
        printf(" 未找到日志文件，首次运行将新建。\n");
    }

    //财务
    fp = fopen("money.txt", "r");
    if (fp != NULL) {
        moneyCount = 0;
        while (moneyCount < MAX_GET_MONEY &&
            fscanf(fp, "%[^\t]\t%lf\t%[^\n]\n",
                money[moneyCount].date,
                &money[moneyCount].charge,
                money[moneyCount].source) == 3) {
            moneyCount++;
        }
        fclose(fp);
        printf(" 已读取财务数据 (%d条)\n", moneyCount);
    }
    else {
        printf(" 未找到财务文件，首次运行将新建。\n");
    }
}

void saveFile() {
    FILE* fp;
    int i;

    //会员
    fp = fopen("vips.txt", "w");
    if (fp == NULL) {
        printf("ERROR：无法保存会员文件！\n");
    }
    else {
        for (i = 0; i < VIPCount; i++) {
            fprintf(fp, "%d\t%s\t%s\t%s\t%.2f\t%d\n",
                vips[i].cardID,
                vips[i].name,
                vips[i].phone,
                vips[i].gender,
                vips[i].balance,
                vips[i].status);
        }
        fclose(fp);
        printf(" 会员数据已保存 (%d条)\n", VIPCount);
    }

    //登记
    fp = fopen("logs.txt", "w");
    if (fp == NULL) {
        printf("ERROR：无法保存日志文件！\n");
    }
    else {
        for (i = 0; i < logCount; i++) {
            fprintf(fp, "%d\t%s\t%s\t%s\t%s\t%.2f\t%.2f\n",
                logs[i].cardID,
                logs[i].name,
                logs[i].phone,
                logs[i].enTime,
                logs[i].exTime,
                logs[i].stHours,
                logs[i].charge);
        }
        fclose(fp);
        printf(" 日志数据已保存 (%d条)\n", logCount);
    }

    //财务
    fp = fopen("money.txt", "w");
    if (fp == NULL) {
        printf("ERROR：无法保存财务文件！\n");
    }
    else {
        for (i = 0; i < moneyCount; i++) {
            fprintf(fp, "%s\t%.2f\t%s\n",
                money[i].date,
                money[i].charge,
                money[i].source);
        }
        fclose(fp);
        printf(" 财务数据已保存 (%d条)\n", moneyCount);
    }
}

//----------------------日志操作 

void getCurTime(char* buf) {
    time_t t = time(NULL);                 // 获取当前时间戳
    struct tm* tm_info = localtime(&t);    // 转成当地时间
    strftime(buf, 20, "%Y-%m-%d %H:%M", tm_info); // 格式化时间
}

void addLog() {
    struct log l;
    int isVIP, i;
    char input[10];

    printf("是否为会员？(1=是 0=否)：");
    scanf("%s", input);
    while (getchar() != '\n');

    isVIP = atoi(input);
    if (isVIP != 0 && isVIP != 1) {
        printf("输入错误！请输入 1（会员）或 0（非会员）！\n");
        Sleep(2000);
        return;
    }

    if (isVIP) {
        printf("请输入会员卡号：");
        scanf("%d", &l.cardID);
        while (getchar() != '\n');

        for (i = 0; i < VIPCount; i++) {
            if (vips[i].cardID == l.cardID) break;
        }
        if (i == VIPCount) {
            printf("未找到该会员！\n");
            Sleep(2000);
            return;
        }
        strcpy(l.name, vips[i].name);
        strcpy(l.phone, vips[i].phone);
    }
    else {
        l.cardID = 0;
        printf("请输入姓名：");
        scanf("%s", l.name);
        while (getchar() != '\n');

        printf("请输入手机号：");
        scanf("%s", l.phone);
        while (getchar() != '\n');
    }

    getCurTime(l.enTime);
    strcpy(l.exTime, "---");
    l.stHours = 0;
    l.charge = 0;
    logs[logCount++] = l;
    printf("入场成功！入场时间：%s\n", l.enTime);
    Sleep(2000);
}

int my_strptime(char* s, struct tm* tm) {
    int n = sscanf(s, "%d-%d-%d %d:%d",
        &tm->tm_year,
        &tm->tm_mon,
        &tm->tm_mday,
        &tm->tm_hour,
        &tm->tm_min);
    if (n != 5) return 0;
    tm->tm_year -= 1900;
    tm->tm_mon -= 1;
    tm->tm_sec = 0;
    return 1;
}

void checkOut() {
    //我一般不放心用AI,但现在已经01:19了,力竭(-呃啊-) _Laia
    int id, i, found = 0;
    char now[20];

    printf("请输入卡号（非会员输入0）：");
    scanf("%d", &id);

    // 非会员
    if (id == 0) {
        // 未离场的非会员
        int uncheckoutIndices[MAX_LOGS];
        int count = 0;
        for (i = 0; i < logCount; i++) {
            if (logs[i].cardID == 0 && strcmp(logs[i].exTime, "---") == 0) {
                uncheckoutIndices[count++] = i;
            }
        }

        if (count == 0) {
            printf("当前没有未离场的非会员记录！\n");
            Sleep(2000);
            return;
        }

        // 显示列表
        printf("\n当前未离场的非会员记录：\n");
        for (i = 0; i < count; i++) {
            int idx = uncheckoutIndices[i];
            printf("%d. 姓名：%s  手机号：%s  入场时间：%s\n",
                i + 1, logs[idx].name, logs[idx].phone, logs[idx].enTime);
        }
        printf("请选择要离场的序号（1~%d）：", count);
        int choice;
        scanf("%d", &choice);
        if (choice < 1 || choice > count) {
            printf("无效选择！\n");
            Sleep(2000);
            return;
        }
        // 记录索引
        i = uncheckoutIndices[choice - 1];
        found = 1;
    }
    else {
        //会员
        for (i = logCount - 1; i >= 0; i--) {
            if (logs[i].cardID == id && strcmp(logs[i].exTime, "---") == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("未找到该卡号未离场的记录！\n");
            Sleep(2000);
            return;
        }
    }

    // 结算逻辑
    getCurTime(now);
    strcpy(logs[i].exTime, now);

    // 计算停留时长
    struct tm tm_en, tm_ex;
    if (!my_strptime(logs[i].enTime, &tm_en) || !my_strptime(now, &tm_ex)) {
        printf("时间解析失败！\n");
        Sleep(2000);
        return;
    }

    time_t t_en = mktime(&tm_en);
    time_t t_ex = mktime(&tm_ex);
    double minutes = difftime(t_ex, t_en) / 60.0;

    logs[i].stHours = minutes / 60.0;

    // 按半小时计费（不足半小时按半小时算）
    int units = (int)(minutes / 30);
    if (minutes > units * 30) units++;
    double charge = units * (PRICE_PER_HOUR / 2.0);

    // 会员
    if (id != 0) {
        charge *= 0.8;
    }

    logs[i].charge = charge;

    printf("停留时长：%.2f 小时\n", logs[i].stHours);
    printf("应收金额：%.2f 元\n", charge);

    // 记录财务
    if (moneyCount >= MAX_GET_MONEY) {
        printf("财务记录已满，无法记录本次收入！\n");
        Sleep(2000);
        return;
    }
    strcpy(money[moneyCount].date, now);
    money[moneyCount].charge = charge;
    sprintf(money[moneyCount].source, "%s", (id == 0) ? "散客" : "会员");
    moneyCount++;

    // 会员扣费
    if (id != 0) {
        for (int j = 0; j < VIPCount; j++) {
            if (vips[j].cardID == id) {
                if (vips[j].balance < charge) {
                    printf("余额不足！实际扣款 %.2f，当前余额 %.2f\n", charge, vips[j].balance);
                    vips[j].balance = 0;
                    Sleep(2000);
                }
                else {
                    vips[j].balance -= charge;
                }
                printf("会员扣费成功，剩余余额：%.2f\n", vips[j].balance);
                Sleep(2000);
                break;
            }
        }
    }

    printf("离场结算完成！\n");
    Sleep(2000);
}

void listLog() {
    if (logCount == 0) {
        printf("当前没有任何登记记录！\n");
        Sleep(2000);
        return;
    }

    printf("\n============================ 登记记录 ============================\n");
    printf("卡号\t姓名\t手机号\t入场时间\t\t离场时间\t\t时长(h)\t收费\n");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < logCount; i++) {
        printf("%d\t%s\t%s\t%s\t%s\t%.2f\t%.2f\n",
            logs[i].cardID,
            logs[i].name,
            logs[i].phone,
            logs[i].enTime,
            logs[i].exTime,
            logs[i].stHours,
            logs[i].charge);
    }
    printf("====================================================================\n");
    Sleep(5000);
}

//---------------------财务操作 
void addMoney() {
    if (moneyCount >= MAX_GET_MONEY) {
        printf("财务记录已满（最多%d条），无法继续添加！\n", MAX_GET_MONEY);
        Sleep(2000);
        return;
    }

    char now[20];
    getCurTime(now);
    strcpy(money[moneyCount].date, now);

    printf("请输入收入金额：");
    scanf("%lf", &money[moneyCount].charge);

    if (money[moneyCount].charge < 0) {
        printf("金额不能为负数！\n");
        Sleep(2000);
        return;
    }

    printf("请输入收入来源（如：商品销售、充值等）：");
    scanf("%s", money[moneyCount].source);

    moneyCount++;
    printf("收入记录已添加！\n");
    Sleep(2000);
}

void listFinance() {
    if (moneyCount == 0) {
        printf("当前没有任何财务记录！\n");
        Sleep(2000);
        return;
    }

    printf("\n==================== 财务记录 ====================\n");
    printf("日期\t\t收入金额\t来源\n");
    printf("--------------------------------------------------\n");
    double total = 0;
    for (int i = 0; i < moneyCount; i++) {
        printf("%s\t%.2f\t\t%s\n",
            money[i].date,
            money[i].charge,
            money[i].source);
        total += money[i].charge;
    }
    printf("--------------------------------------------------\n");
    printf("合计：%.2f 元\n", total);
    printf("==================================================\n");
    Sleep(5000);
}

void financeSummary() {
    if (moneyCount == 0) {
        printf("当前没有任何财务记录！\n");
        Sleep(2000);
        return;
    }

    double total = 0;
    double vipTotal = 0;    // 会员收入
    double guestTotal = 0;  // 散客
    double otherTotal = 0;  // 其他

    for (int i = 0; i < moneyCount; i++) {
        total += money[i].charge;
        if (strcmp(money[i].source, "会员") == 0) {
            vipTotal += money[i].charge;
        }
        else if (strcmp(money[i].source, "散客") == 0) {
            guestTotal += money[i].charge;
        }
        else {
            otherTotal += money[i].charge;
        }
    }

    printf("\n==================== 收入统计 ====================\n");
    printf("总收入：%.2f 元\n", total);
    printf("其中：\n");
    printf("  会员收入：%.2f 元 (占比 %.1f%%)\n", vipTotal, (total > 0 ? vipTotal / total * 100 : 0));
    printf("  散客收入：%.2f 元 (占比 %.1f%%)\n", guestTotal, (total > 0 ? guestTotal / total * 100 : 0));
    if (otherTotal > 0) {
        printf("  其他收入：%.2f 元 (占比 %.1f%%)\n", otherTotal, (total > 0 ? otherTotal / total * 100 : 0));
    }
    printf("==================================================\n");
    Sleep(3000);
}