function checkUseConfigChanged(){
    if((userConfigSrc.configSetting.typeName===mg_curTypeName)&&
            (userConfigSrc.configSetting.modelName===mg_curModeName)&&
            (userConfigSrc.configSetting.version===mg_curVersion)){
        return false;
    }
    else{
        return true;
    }

}
function saveUserConfig(){
    userConfigSrc.setTypeId(mg_curTypeId);
    userConfigSrc.setTypeName(mg_curTypeName);
    userConfigSrc.setModelName(mg_curModeName);
    userConfigSrc.setAxisCount(mg_curAxisCount);
    userConfigSrc.setVersion(mg_curVersion);
}

function reloadControlCmdFile(){
    mg_servoCmd.loadExtendTree("file:///"+sysConfigFilePathSrc+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/PrmFuncExtension.xml")
    mg_servoCmd.loadCmdTree("file:///"+sysConfigFilePathSrc+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/PrmFuncCmd.xml");
    mg_servoCmd.loadAdvanceFastAccessCommandTree("file:///"+sysConfigFilePathSrc+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/AdvanceFastCommand.xml");
    mg_controlKeyName.source="./ConfigPages/Resource/DeviceData/"+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/ControlName.json";
}
