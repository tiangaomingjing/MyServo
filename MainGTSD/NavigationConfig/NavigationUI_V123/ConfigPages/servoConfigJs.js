//---------找到当前配置的树index-----------------------
function findCurrentConfig(model){
    var type=userConfigSrc.configSetting.typeName;
    var modelName=userConfigSrc.configSetting.modelName;
    var version=userConfigSrc.configSetting.version;
    var theIndex;
    var resultIndex;
    console.log("source:"+type+modelName+version);
    for(var i=0;i<model.childrenCount();i++){
        theIndex=model.index(i,0)
        if(model.data(theIndex,TreeModel.TEXT1)===type){
            resultIndex=_findModelName(model,theIndex,modelName,version);
            break;
        }
        console.log(model.data(theIndex,TreeModel.TEXT1));
    }
    var modelIndex=m_treeModel.parent(resultIndex);
    var typeIndex=m_treeModel.parent(modelIndex);
    console.log(qsTr("type: %1 ,model: %2 ,firmware: %3")
                .arg(m_treeModel.data(typeIndex,TreeModel.TEXT1))
                .arg(m_treeModel.data(modelIndex,TreeModel.TEXT1))
                .arg(m_treeModel.data(resultIndex,TreeModel.TEXT1)))
    console.log(model.data(resultIndex,TreeModel.TEXT1));
    return resultIndex;
}
function _findModelName(model,currentIndex,modelName,version){
    var qIndex;
    var ret;
    for(var i=0;i<model.childrenCount(currentIndex);i++){
        qIndex=model.index(i,0,currentIndex);
        console.log(model.data(qIndex,TreeModel.TEXT1));
        if(model.data(qIndex,TreeModel.TEXT1)===modelName){
            ret=_findVersion(model,qIndex,version);
            break;
        }
    }
    return ret;
}
function _findVersion(model,currentIndex,aVersion){
    var qIndex;
    for(var i=0;i<model.childrenCount(currentIndex);i++){
        qIndex=model.index(i,0,currentIndex);
        if(model.data(qIndex,TreeModel.TEXT1)===aVersion){
//            servoConfig.theIndex=qIndex;
            break;
        }
    }
    return qIndex;
//    console.log("the Index text:"+model.data(servoConfig.theIndex,TreeModel.TEXT1));
////        console.log(servoConfig.theIndex);
//    console.log(servoConfig.theIndex);
//    var modelIndex=m_treeModel.parent(servoConfig.theIndex);
//    var typeIndex=m_treeModel.parent(modelIndex);
//    console.log(qsTr("the index:type %1 model%2 firmware%3")
//                .arg(m_treeModel.data(typeIndex,TreeModel.TEXT1))
//                .arg(m_treeModel.data(modelIndex,TreeModel.TEXT1))
//                .arg(m_treeModel.data(servoConfig.theIndex,TreeModel.TEXT1)))
}
//---------找到当前配置的树index-----------------------end

function getVersionText(){
    var str;
    str=m_treeModel.data(m_tree.currentIndex,TreeModel.TEXT1);
    return str;
}
function getModelText(){
    var str;
    var parentIndex;
    parentIndex=m_treeModel.parent(m_tree.currentIndex);
    str=m_treeModel.data(parentIndex,TreeModel.TEXT1);
    return str;
}
function getTypeText(){
    var str;
    var modelIndex;
    var typeIndex;
    modelIndex=m_treeModel.parent(m_tree.currentIndex);
    typeIndex=m_treeModel.parent(modelIndex);
    str=m_treeModel.data(typeIndex,TreeModel.TEXT1);
    return str;
}
function getAxisCount(){
    var str;
    str=m_treeModel.data(m_tree.currentIndex,TreeModel.TEXT2);
    return parseInt(str);
}
function getTypeId(){
    var str;
    var modelIndex;
    var typeIndex;
    modelIndex=m_treeModel.parent(m_tree.currentIndex);
    typeIndex=m_treeModel.parent(modelIndex);
    str=m_treeModel.data(typeIndex,TreeModel.TEXT2);
    return parseInt(str);
}

//改变当前的配置
//function saveUserConfig(){
//    userConfigSrc.setTypeId(servoConfig.m_configTypeId);
//    userConfigSrc.setTypeName(servoConfig.m_configTypeName);
//    userConfigSrc.setModelName(servoConfig.m_configModelName);
//    userConfigSrc.setAxisCount(servoConfig.m_configAxisCount);
//    userConfigSrc.setVersion(servoConfig.m_configVersion);
//}

//获得设置描述信息
function showDecribeString(index){
    var str=new Array;
    str.push("类型："+m_treeModel.data(m_treeModel.parent(index),TreeModel.TEXT1));
    str.push("固件版本："+m_treeModel.data(index,TreeModel.TEXT1));
    var roleIndex=new Array;
    roleIndex.push(TreeModel.TEXT1);
    roleIndex.push(TreeModel.TEXT2);
    roleIndex.push(TreeModel.TEXT3);
    roleIndex.push(TreeModel.TEXT4);
    roleIndex.push(TreeModel.TEXT5);
    roleIndex.push(TreeModel.TEXT6);
    roleIndex.push(TreeModel.TEXT7);
    roleIndex.push(TreeModel.TEXT8);
    for(var i=1;i<m_treeModel.columnCount();i++)
        str.push(m_treeModel.headerData(i,TreeModel.HORIZONTAL)+":"+m_treeModel.data(index,roleIndex[i]));
    return str.join("\n\n");
}

function updateDecribeString(){
    m_servoInformationText=qsTr("型号：%2\n\n控制轴数:%3\n\n输入电源：%4\n\n额定功率:%5\n\nIO信息：\n\nDI:%6 DO:%7\n\nAI:%8 AO:%9\n")
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).model)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).axisCount)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).powerIn)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).power)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).DI)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).DO)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).AI)
                        .arg(servoTypeDataModel.get(m_servoConfigTypesListView.currentIndex).AO)
}
