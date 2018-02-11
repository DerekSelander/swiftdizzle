//
//  ViewController.swift
//  swiftdizzle
//
//  Created by Derek Selander on 2/10/18.
//  Copyright © 2018 Razeware. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

  override func viewDidLoad() {
    super.viewDidLoad()

    rebind_swiftClass("swiftdizzle.ViewController",
                      "_T011swiftdizzle14ViewControllerC12someFunctionyyF",
                      "swiftdizzle.ViewController",
                      "_T011swiftdizzle14ViewControllerC15omagerdFunctionyyF",
                      true)
    print("In \(#function) about to call self.someFunction()...")
    self.someFunction()
  }

  func someFunction() {
    print(#function)
  }
  
  func omagerdFunction() {
    print(#function)
    self.omagerdFunction()
  }
}

