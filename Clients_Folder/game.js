import net from "net"

function play(){

    var options = ['rock', 'paper', 'scissors', 'lizard', 'spock'], toWin = ['scissorspaper', 'paperrock', 'rocklizard', 'lizardspock', 'spockscissors', 'rockscissors', 'scissorslizard', 'lizardpaper', 'paperspock', 'spockrock'];
    var user = '', cpu = '', myScore = 0;;
    var userChoice = $('.user-choice')[0];
    var pickedChoice = $('.picked')[0];
    var userPick = $('.user-pick')[0];
    var pcPick = $('.pc-pick')[0];
    var resultElement = $('.result')[0];
    var resultTitle = $(resultElement).find('.title')[0];
    var scoreCount = $('.score-count')[0];
    update();

    
    function start(){
        winner(user, cpu);
        userChoice.classList.add('hidden');
        pickedChoice.classList.remove('hidden');
        clear();
        build(true, user);
        build(false, cpu);
    }


    function getComputerChoice(){
        return options[Math.floor(Math.random()*5)];
    }
    
    function getUserChoice(target){
        if (target.nodeName === 'IMG'){
            return target.parentElement.classList[1];
        }
        return target.classList[1];
    }
    
    function winner(user, comp){
        if (user === comp){
            resultTitle.innerText = 'Tie';
        }else if(getUserWinsStatus(user + comp)){
            resultTitle.innerText = 'You win';
            addScore(1);
        }else{
            resultTitle.innerText = 'You lose';
            addScore(-1);
        }
    }

    function getUserWinsStatus(myString){
        return toWin.some(winString => winString === myString);
    }

    function build(isUser, className){
        var it = document.createElement('div');
        it.classList = [`game-card ${className}`];
        it.innerHTML = `<img src="images/icon-${className}.svg" alt="${className}">`;
        if(isUser){
            userPick.append(it);
        }else{
            pcPick.append(it);
        }
    }

    function clear(){
        userPick.innerHTML = '';
        pcPick.innerHTML = '';
    }

    function addScore(round){
        myScore += round;
        if(myScore < 0){
            myScore = 0;
        }
        update();
    }

    function rScore(){
        var score = +window.localStorage.getItem('gameScore') || 0;
        myScore = score;
        update();
    }

    function update(){
        scoreCount.innerText = myScore;
        window.localStorage.setItem('gameScore', myScore);
    }

    var rulesBtn = $('.rules-btn')[0];
    var rulesSpanBackground = $('.modal-bg')[0];
    var rulesSpan = $('.modal')[0];

    $(rulesBtn).click(function(){
        rulesSpan.classList.add('active');
        rulesSpanBackground.classList.add('active');
    });

    $(rulesSpanBackground).click(function(event){
        if(event.target === rulesSpanBackground){
            hide();
        }
    });

    $(".close").click(hide);

    function hide(){
        rulesSpan.classList.remove('active');
        rulesSpanBackground.classList.remove('active');  
    }
    
    window.addEventListener('load', function(){
        rScore();
    
        $('.user-choice .game-card').toArray().forEach(function(card){
            $(card).click(function(event){
                user = getUserChoice(event.target);
                //Mandas user a server (String)
                /*
                const server = net.createServer((socket) => {
                    console.log("Connection from", socket.remoteAddress, "port", socket.remotePort)
                    socket.on("connect", () => {
                        socket.write(user)
                    })
                    socket.on("data", (data) => {
                        cpu = options[parseInt(data)]  
                    })
                    socket.on("end", () => {
                      console.log("Closed", socket.remoteAddress, "port", socket.remotePort)
                    })
                })*/
                cpu=getComputerChoice();
                start();
            })
        });

        $($(resultElement).find('button')[0]).click(function(){
            userChoice.classList.remove('hidden');
            pickedChoice.classList.add('hidden');
        });
    });
}

play();