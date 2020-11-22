package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"sort"
	"time"
)

const exeName = "minisat" //"../build/Release/NimmerSAT"
const argPath = "2007SATindustrial/"
const timeoutMs = 20000

func measureRuntime(exe, path string) (float64, bool) {
	p2 := exec.Command(exe, path)
	done := make(chan error, 1)
	p2.Stdout = os.Stdout
	p2.Stderr = os.Stderr

	tStart := time.Now()
	err := p2.Start()
	if err != nil {
		fmt.Println("Start: ", err)
		return 0, false
	}

	go func() {
		done <- p2.Wait()
	}()

	select {
	case <-time.After(timeoutMs * time.Millisecond):
		fmt.Println("Timeout")
		p2.Process.Kill()
		return 0, false

	case err := <-done:
		if err != nil && 
		   err.Error() != "exit status 10" &&
		  err.Error() != "exit status 20" {  // picosat, minisat UNSAT
			fmt.Println("Process failed!", err)
			return 0, false
		}
		return time.Now().Sub(tStart).Seconds(), true
	}
}

func main() {
	timeArr := make([]float64, 0)

	files, err := ioutil.ReadDir(argPath)
	if err != nil {
		fmt.Println("Error on reading directory")
		return
	}
	for _, f := range files {
		fmt.Print(f.Name(), "\t\t")
		time, s := measureRuntime(exeName, argPath+f.Name())
		if s {
			timeArr = append(timeArr, time)
		}
	}
	sort.Float64s(timeArr)
	f, err := os.Create("measurements/2007SATindustrial_minisat.data")
	if err != nil {
		fmt.Println(err)
		return
	}
	defer f.Close()

	for _, v := range timeArr {
		f.WriteString(fmt.Sprintln(v))
	}
}
