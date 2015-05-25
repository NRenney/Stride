
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

#include "pythonproject.h"
#include "codevalidator.h"


PythonProject::PythonProject(QString platformPath,
                             QString projectDir,
                             QString pythonExecutable) :
    Builder(projectDir, platformPath),
    m_runningProcess(this)

{
    if(pythonExecutable.isEmpty()) {
        m_pythonExecutable = "python";
    } else {
        m_pythonExecutable = pythonExecutable;
    }
}

PythonProject::~PythonProject()
{
    m_running.store(0);
    m_runningProcess.kill();
    m_runningProcess.waitForFinished();
}

void PythonProject::build(AST *tree)
{
    writeAST(tree);
    QProcess pythonProcess(this);
    QStringList arguments;
    pythonProcess.setWorkingDirectory(m_platformPath + QDir::separator() + "scripts");
    arguments << "build.py" << m_projectDir;
    pythonProcess.start(m_pythonExecutable, arguments);
    if(!pythonProcess.waitForFinished()) {

    }
    QByteArray stdOut = pythonProcess.readAllStandardOutput();
    QByteArray stdErr = pythonProcess.readAllStandardError();
    emit outputText(stdOut);
    emit errorText(stdErr);
//    qDebug() << stdOut;
//    qDebug() << stdErr;
}

void PythonProject::run(bool pressed)
{
    if (!pressed) {
        stopRunning();
        return;
    }
    QStringList arguments;
    if (m_runningProcess.state() == QProcess::Running) {
       m_runningProcess.close();
       if (!m_runningProcess.waitForFinished(5000)) {
           qDebug() << "Could not stop running process. Not starting again.";
           return;
       }
    }
    m_runningProcess.setWorkingDirectory(m_platformPath + QDir::separator() + "scripts");
    arguments << "run.py" << m_projectDir;
    m_runningProcess.start(m_pythonExecutable, arguments);
    m_running.store(1);
    while(m_running.load() == 1) {
        if(m_runningProcess.waitForFinished(50)) {
            m_running.store(0);
        }
        QByteArray stdOut = m_runningProcess.readAllStandardOutput();
        QByteArray stdErr = m_runningProcess.readAllStandardError();
        emit outputText(stdOut);
        emit errorText(stdErr);
        qApp->processEvents();
    }
    QByteArray stdOut = m_runningProcess.readAllStandardOutput();
    QByteArray stdErr = m_runningProcess.readAllStandardError();
    emit outputText(stdOut);
    emit errorText(stdErr);
//    qDebug() << m_runningProcess.readAllStandardOutput();
//    qDebug() << m_runningProcess.readAllStandardError();
    emit outputText("Done.");
    m_runningProcess.close();
}

void PythonProject::stopRunning()
{
    m_running.store(0);
}

void PythonProject::writeAST(AST *tree)
{
    QFile saveFile(m_projectDir + QDir::separator() + "tree.json");

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonArray treeObject;
    foreach(AST *node, tree->getChildren()) {
        QJsonObject nodeObject;
        if (node->getNodeType() == AST::Platform) {
            nodeObject["platform"] = QString::fromStdString(static_cast<PlatformNode *>(node)->platformName());
        } else if (node->getNodeType() == AST::Stream) {
            m_curStreamArray = QJsonArray();
            streamToJsonArray(static_cast<StreamNode *>(node));
            nodeObject["stream"] = m_curStreamArray;
        } else if (node->getNodeType() == AST::Block) {
            QJsonObject blockObj;
            BlockNode *block = static_cast<BlockNode *>(node);
            blockObj["name"] = QString::fromStdString(block->getName());
            blockObj["type"] = QString::fromStdString(block->getObjectType());
            vector<PropertyNode *> props = block->getProperties();
            QJsonObject propertiesObj;
            foreach(PropertyNode *prop, props) {
                AST *propValue = prop->getValue();
                if (propValue->getNodeType() == AST::Int) {
                    propertiesObj[QString::fromStdString(prop->getName())]
                            = static_cast<ValueNode *>(propValue)->getIntValue();
                } else if (propValue->getNodeType() == AST::Real) {
                    propertiesObj[QString::fromStdString(prop->getName())]
                            = static_cast<ValueNode *>(propValue)->getRealValue();
                } else if (propValue->getNodeType() == AST::String) {
                    propertiesObj[QString::fromStdString(prop->getName())]
                            = QString::fromStdString(static_cast<ValueNode *>(propValue)->getStringValue());
                } else if (propValue->getNodeType() == AST::Expression) {
                        // TODO complete this
                }
            }
            blockObj["ports"] = propertiesObj;
            nodeObject["block"] = blockObj;
        } else if (node->getNodeType() == AST::BlockBundle) {
            QJsonObject blockObj;
            BlockNode *block = static_cast<BlockNode *>(node);
            BundleNode *bundle = block->getBundle();
            blockObj["name"] = QString::fromStdString(bundle->getName());
            blockObj["type"] = QString::fromStdString(block->getObjectType());
            AST *bundleIndex = bundle->index();
            Q_ASSERT(bundleIndex->getNodeType() == AST::Int || bundleIndex->getNodeType() == AST::Real);
            blockObj["size"] = static_cast<ValueNode *>(bundleIndex)->getIntValue();
            vector<PropertyNode *> props = block->getProperties();
            QJsonObject propertiesObj;
            foreach(PropertyNode *prop, props) {
                AST *propValue = prop->getValue();
                if (propValue->getNodeType() == AST::Int) {
                    propertiesObj[QString::fromStdString(prop->getName())]
                            = static_cast<ValueNode *>(propValue)->getIntValue();
                }
            }
            blockObj["ports"] = propertiesObj;
            nodeObject["blockbundle"] = blockObj;
        }
        treeObject.append(nodeObject);
    }
    QJsonDocument saveDoc(treeObject);
    saveFile.write(saveDoc.toJson());
}

void PythonProject::astToJson(AST *node, QJsonObject &obj)
{
    if (node->getNodeType() == AST::Bundle) {
        obj["type"] = "Bundle";
        obj["name"] = QString::fromStdString(static_cast<BundleNode *>(node)->getName());
        AST *indexNode = static_cast<BundleNode *>(node)->index();
        if (indexNode->getNodeType() == AST::Int) {
            obj["index"] = static_cast<ValueNode *>(indexNode)->getIntValue();
        }
        obj["rate"] = node->getRate();
    } else if (node->getNodeType() == AST::Name) {
        obj["type"] = "Name";
        obj["name"] = QString::fromStdString(static_cast<BundleNode *>(node)->getName());
        obj["rate"] = node->getRate();
    } else if (node->getNodeType() == AST::Expression) {
        obj["type"] = "Expression";
        obj["rate"] = node->getRate();
    } else if (node->getNodeType() == AST::Function) {
        obj["type"] = "Function";
        functionToJson(static_cast<FunctionNode *>(node), obj);
    } else if (node->getNodeType() == AST::Stream) {
        obj = QJsonObject();
        streamToJsonArray(static_cast<StreamNode *>(node));
    } else if (node->getNodeType() == AST::Int) {
        obj["type"] = "Value";
        obj["value"] = static_cast<ValueNode *>(node)->getIntValue();
    } else if (node->getNodeType() == AST::Real) {
        obj["type"] = "Value";
        obj["value"] = (qreal) static_cast<ValueNode *>(node)->getRealValue();
        qDebug() << obj["value"].isDouble() << obj["value"].toDouble();

    } else if (node->getNodeType() == AST::String) {
        obj["type"] = "Value";
        obj["value"] = QString::fromStdString(static_cast<ValueNode *>(node)->getStringValue());
    } else {
        obj["type"] = "Unsupported";
    }
}

void PythonProject::streamToJsonArray(StreamNode *node)
{
    addNodeToStreamArray(node->getLeft());
    addNodeToStreamArray(node->getRight());
}

void PythonProject::functionToJson(FunctionNode *node, QJsonObject &obj)
{
    obj["name"] = QString::fromStdString(node->getName());
    vector<PropertyNode *> properties = node->getProperties();
    QJsonObject propObject;
    foreach(PropertyNode *property, properties) {
        QJsonObject propValue;
        astToJson(property->getValue(), propValue);
        propObject[QString::fromStdString(property->getName())] = propValue;
    }
    obj["properties"] = propObject;
    obj["rate"] = node->getRate();
}

void PythonProject::addNodeToStreamArray(AST *node)
{
    QJsonObject obj;
    astToJson(node, obj);
    if(!obj.isEmpty()) {
        m_curStreamArray.append(obj);
    }
}


bool PythonProject::isValid()
{
    return true;
}
