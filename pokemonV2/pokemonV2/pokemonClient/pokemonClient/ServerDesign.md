# socket设计

## Server
IP地址与端口都是公开的

### 关键逻辑使用TCP

    1. 监听固定端口
    2. 与client建立连接
    3. 登录/注册
        3.1 注册
         3.1.1 `Client` 发出注册请求  
           填好用户名及密码
    
         3.1.2 Server 收到请求进行响应
           判断是否用户名是否合法
           判断用户名是否重复
           判断密码复杂性
           随机分配三个小精灵
           加入数据库
           发回响应
     
        3.2 登录
         3.2.1 Client 发出登录请求
    
    4. 数据查询
    5. 加入房间
    6. 开始游戏
    7. 结束游戏
    8. 登出
    9. 关闭连接 


* 非关键逻辑使用UDP


## Client

# socket设计

## Client
IP地址与端口都是公开的

### 关键逻辑使用TCP
    1. 监听固定端口
    2. 与对应client建立连接
    3. 登录
    4. 数据查询
    5. 加入房间
    6. 开始游戏
    7. 结束游戏
    8. 登出



### 非关键逻辑使用UDP

##  数据包格式规定

| 类型                |               | 包内格式                              |
| ----------------- | :------------ | --------------------------------- |
| SIGN_IN           | 登录            | \|`type`\| `account` \| `password` \| 
| SIGN_IN_SUCCESS   | 登录成功          | \|`type`\|`clientInfo`|`pet(list)`\| 
| SIGN_IN_FAIL      | 登录失败          | \|`type`\|`reason`\|                 |
| SIGN_UP           | 注册            | \|`type`\|`account`\|`password`\|     |
| SIGN_UP_SUCCESS   | 注册成功          | \|`type`\|`pet(list)`\|              |
| SIGN_UP_FAIL      | 注册失败          |\| `type`\|`reason`\|                 |
| LOG_OUT           | 登出            | \|`type`\|`account`\|                |
| LOG_OUT_ACK       | 登出确认          | \|`type`\|`account`\|                |
| SERVER_ERROR      | 服务器错误         | \|`type`\|`reason`\|                 |
| GET_ONLINE_CLIENT | 获取当前在线用户及人数请求 | \|`type`\|                          |
| ONLINE_CLIENT_ACK | 当前在线用户及人数     | \|`type`\|`num`\|`user(list)`\|       |
| GET_CLIENT_PET    | 查看某用户的小精灵     | \|`type`\|`name`\|                   |
| CLIENT_PET_ACK        | 某用户的小精灵       | \|`type`\|`pet(list)`\|              |


### 初始小精灵属性
|名称|类型|等级|当前经验值|最大经验值|当前生命值|最大生命值|物理攻击|魔法攻击|物理防御|魔法防御|攻击速度|暴击率|移动速度|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|name|type|level|exp|expMax|health|healthMax|physicalAtk|magicAtk|physicalDef|magicDef|atkSpeed|critRate|speed|





