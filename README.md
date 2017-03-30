# Arduino Task Scheduler

A idéia desta biblioteca é facilitar o desenvolvimento de projetos com mais de uma funcionalidade, para tal, cada funcionalidade/tarefa é chamada de Task e implementada na classe AOTask, que é escalonada pelo AOScheduler.

Mais informações podem ser obtidas em https://phcco.com/arduino-task-scheduler

## Instalação

Baixe os arquivos e copie a pasta AOScheduler para dentro da pasta de plugins da sua IDE de desenvolvimento.

## Princípios da biblioteca

Para exemplificar a sua utilização, montei um exemplo onde é possível determinar que um LED fique aceso durante X tempo e desligado Y tempo. O código do MultiBlink pode ser visto abaixo (uma versão com comentários também está dentro da biblioteca).

```
#include "AOScheduler.h"

class MultiBlink : public AOTask {
	private:
		int _pin;
		unsigned long _uptime,_downtime;
		int _state;
    public: 
		MultiBlink(int pin,unsigned long uptime, unsigned long downtime){
			_pin = pin;
			_uptime = uptime;
			_downtime = downtime;
			_state = LOW;
		}

		int setup(){
			this->task_expected_period = _uptime;
			pinMode(_pin, OUTPUT);    
			digitalWrite(_pin,_state = HIGH);
		}

		int loop(){
			if(_state==LOW){
				this->task_expected_period = _uptime;
				_state = HIGH;
			}else{
				this->task_expected_period = _downtime;
				_state = LOW;
			}
			digitalWrite(_pin, _state);
			return AO_OK;
		}  
};

AOScheduler aos;
// Tempos em microsegundos
// MultiBlink blinker1(pin, tempo ligado, tempo desligado);
MultiBlink blinker1(13,SECOND,2*SECOND);
MultiBlink blinker2(12,MILLISECOND,1*SECOND);
MultiBlink blinker3(11,MILLISECOND,1*SECOND);
MultiBlink blinker4(9,SECOND/2,3*SECOND);
MultiBlink blinker5(8,MILLISECOND,1*SECOND);
MultiBlink blinker6(5,SECOND,4*SECOND);

void setup(){
	aos.add(&blinker1);
	aos.add(&blinker2);
	aos.add(&blinker3);
	aos.add(&blinker4);
	aos.add(&blinker5);
	aos.add(&blinker6);
}

void loop(){
	aos.run();
}
```

Na linha 1 incluímos a biblioteca AOScheduler, entre a linha 3 e 33 declaramos o comportamento da tarefa. Nesta tarefa a função `setup` é chamada apenas uma vez (ao ser adicionada ao escalonador) e a função loop é executada cada vez que a tarefa é escalonada. Note que isso faz com que a tarefa tenha uma execução atômica (não é interrompida no meio da execução do loop). 

Por ser atômica, qualquer `delay` ou `delayMicroseconds` dentro da função pode atrasar a execução da próxima tarefa. O escalonador executa a função loop de acordo com o valor em microsegundos armazenado no atributo `task_expected_period`. Se você configurar para executar a cada 1000 microsegundos, o escalonador se esforçará para executar neste período, inclusive ajustando o tempo entre cada tarefa para aproximar o tempo de execução ao desejado.

Na linha 35 criamos o escalonador, e nas linhas 38 até 43 criamos várias tarefas do `MultiBlink`, onde o primeiro parâmetro é o pino digital que será utilizado para enviar o sinal, o segundo é o tempo que o LED deve permanecer ligado e o terceiro é o tempo que o LED deve permanecer desligado.

Na função `setup` adicionamos ao escalonador as tarefas e no `loop` iniciamos a execução do escalonador. O método `run` só retornará quando não houver mais tarefas a serem executadas.

>
O método `loop` das tarefas devem sempre retornar `AO_OK` se a tarefa precisa ser reescalonada, se esta tarefa retornar `AO_EXIT` ela sairá da fila de execução e não será mais executada.


## Algumas observações e trabalhos futuros

Como este escalonador trabalha com timers para executar suas tarefas, não espere que as execuções respeitem fielmente os prazos, é um escalonador de tempo compartilhado e portanto pode falhar.

A idéia agora é melhorar a biblioteca tentando acelerar o processo de escalonamento e ajustar os temporizadores para evitar a desincronia das tarefas, pois o tempo de escalonamento ainda não é contabilizado nestes timers.

